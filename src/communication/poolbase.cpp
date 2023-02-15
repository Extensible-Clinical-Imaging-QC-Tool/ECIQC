

#include "communication/ThreadSafeQueue.hpp"
#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#ifdef WITH_THREADS // Without threads pool does not make sense...

#include "poolbase.h"
#include "dcmtk/dcmnet/diutil.h"

// ----------------------------------------------------------------------------

DQDBaseSCPPool::DQDBaseSCPPool()
  : m_dset(), 
    m_criticalSection(),
    m_workersBusy(),
    m_workersIdle(),
    m_cfg(),
    m_maxWorkers(5),
    m_sourcelist(),
    m_peeraelist(),
    m_runMode( LISTEN )
    // not implemented yet: m_workersBusyTimeout(60),
    // not implemented yet: m_waiting(),
{
}

// ----------------------------------------------------------------------------

DQDBaseSCPPool::~DQDBaseSCPPool()
{
}

// ----------------------------------------------------------------------------

OFCondition DQDBaseSCPPool::listen()
{
  m_runMode = LISTEN;

  /* Copy the config to a shared config that is shared by all workers. */
  DcmSharedSCPConfig sharedConfig(m_cfg);

  /* Copy the list of acceptable IPs/hostnames */
  OFList<OFString>& source_list = m_sourcelist;

  OFList<OFString>& peerAE_list = m_peeraelist;

  /* Initialize network, i.e. create an instance of T_ASC_Network*. */
  T_ASC_Network *network = NULL;
  OFCondition cond = ASC_initializeNetwork( NET_ACCEPTOR, OFstatic_cast(int, m_cfg.getPort()), m_cfg.getACSETimeout(), &network );
  if( cond.bad() )
    return cond;

  /* As long as all is fine (or we have been to busy handling last connection request) keep listening */
  while ( m_runMode == LISTEN && ( cond.good() || (cond == NET_EC_SCPBusy) ) )
  {
    // Reset status
    cond = EC_Normal;
    // Every incoming connection is handled in a new association object
    T_ASC_Association *assoc = NULL;
    // Listen to a socket for timeout seconds for an association request, accepts TCP connection.
    cond = ASC_receiveAssociation( network, &assoc, m_cfg.getMaxReceivePDULength(), NULL, NULL, OFFalse,
        m_cfg.getConnectionBlockingMode(), OFstatic_cast(int, m_cfg.getConnectionTimeout()) );

    /* If we have a connection request, try to find/create a worker to handle it */
    if (cond.good())
    {
      cond = runAssociation(assoc, sharedConfig, source_list, peerAE_list, m_dset);

      /* If anything goes wrong running association: Refuse it */
      if (cond.bad())
      {
        if (cond == NET_EC_SCPBusy)
        {
          rejectAssociation(assoc, ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED);
        }
        else
        {
          rejectAssociation(assoc, ASC_REASON_SP_PRES_TEMPORARYCONGESTION);
        }
        dropAndDestroyAssociation(assoc);
      }
    }

    /* If error occurred while receiving association, clean up */
    else
    {
      /* Handle timeout and errors differently */
      if ( cond == DUL_NOASSOCIATIONREQUEST )
      {
        ASC_destroyAssociation( &assoc );
      }
      else
      {
        dropAndDestroyAssociation(assoc);
        DCMNET_ERROR("DcmBaseSCPPool: Error receiving association: " << cond.text());
      }
      // ... and keep listening ...
      cond = EC_Normal;
    }
  }

  m_criticalSection.lock();
  m_runMode = SHUTDOWN;

  // iterate over all busy workers, join their threads and delete them.
  for
  (
    OFListIterator( DQDBaseSCPPool::DQDBaseSCPWorker* ) it = m_workersBusy.begin();
    it != m_workersBusy.end();
    ++it
  )
  {
    m_criticalSection.unlock();
    (*it)->join();
    delete *it;
    m_criticalSection.lock();
  }

  m_workersBusy.clear();
  m_criticalSection.unlock();

  /* In the end, clean up the rest of the memory and drop network */
  ASC_dropNetwork(&network);

  return EC_Normal;
}

void DQDBaseSCPPool::stopAfterCurrentAssociations()
{
  m_criticalSection.lock();
  if (m_runMode == LISTEN )
    m_runMode = STOP;
  m_criticalSection.unlock();
}

// ----------------------------------------------------------------------------

Uint16 DQDBaseSCPPool::getMaxThreads()
{
  return m_maxWorkers;
}

// ----------------------------------------------------------------------------

OFList<OFString> DQDBaseSCPPool::getacceptableIPs()
{
  return m_sourcelist;
}

// ----------------------------------------------------------------------------

OFList<OFString> DQDBaseSCPPool::getcallingAETitles()
{
  return m_peeraelist;
}

// ----------------------------------------------------------------------------

size_t DQDBaseSCPPool::numThreads(const OFBool onlyBusy)
{
  size_t result = 0;
  m_criticalSection.lock();
  if (!onlyBusy)
  {
    result = m_workersBusy.size() + m_workersIdle.size();
  }
  else
    result = m_workersBusy.size();
  m_criticalSection.unlock();
  return result;
}

// ----------------------------------------------------------------------------

void DQDBaseSCPPool::setMaxThreads(const Uint16 maxWorkers)
{
  m_maxWorkers = maxWorkers;
}

// ----------------------------------------------------------------------------

void DQDBaseSCPPool::setacceptableIPs(OFList<OFString> source_list)
{
  m_sourcelist = source_list;
}

// ----------------------------------------------------------------------------
void DQDBaseSCPPool::setpointer(OFshared_ptr<ThreadSafeQueue<DcmDataset>> dset)
{
    m_dset = dset;
}

// ----------------------------------------------------------------------------

void DQDBaseSCPPool::setcallingAETitles(OFList<OFString> aetitle_list)
{
  m_peeraelist= aetitle_list;
}

// ----------------------------------------------------------------------------

OFCondition DQDBaseSCPPool::runAssociation(T_ASC_Association *assoc,
                                           const DcmSharedSCPConfig& sharedConfig, const OFList<OFString>& sourcelist,
                                           const OFList<OFString>& peerAE_list,
                                           OFshared_ptr<ThreadSafeQueue<DcmDataset>> dset)
{
  /* Try to find idle worker thread */
  OFCondition result = EC_Normal;
  DQDBaseSCPWorker *chosen = NULL;

  /* Do we have idle worker threads that can handle the association? */
  m_criticalSection.lock();
  if (m_workersIdle.empty())
  {
    if (m_workersBusy.size() >= m_maxWorkers)
    {
      /* No idle workers and maximum of busy workers reached? Return busy */
      result = NET_EC_SCPBusy;
    }
    else /* Else we can produce another worker */
    {
      DCMNET_DEBUG("DQDBaseSCPPool: Starting new DcmSCP worker thread");
      DQDBaseSCPWorker* const worker = createSCPWorker();
      if (!worker) /* Oops, we cannot allocate a new worker thread */
      {
        result = EC_MemoryExhausted;
      }
      else /* Configure worker thread */
      {
        m_workersBusy.push_back(worker);
        worker->setSharedConfig(sharedConfig);
        worker->setIPs(sourcelist);
        worker->setpeerAETitles(peerAE_list);
        worker->setdatasetaddress(dset);
        chosen = worker;
      }
    }
  }
  /* Else we have idle workers, use one of them */
  else
  {
    chosen = m_workersIdle.front();
    m_workersIdle.pop_front();
    m_workersBusy.push_back(chosen);
    result = EC_Normal;
  }
  m_criticalSection.unlock();

  /* Hand association to worker */
  if (result.good())
  {
    result = chosen->setAssociation(assoc);
  }
  /* Start the thread */
  if (result.good())
  {
     if (chosen->start() != 0)
     {
       result = NET_EC_CannotStartSCPThread;
     }
  }
  /* Return to listen loop */
  return result;
}

// ----------------------------------------------------------------------------

void DQDBaseSCPPool::rejectAssociation(T_ASC_Association *assoc,
                                       const T_ASC_RejectParametersReason& reason)
{
  T_ASC_RejectParameters rej;
  rej.result = ASC_RESULT_REJECTEDTRANSIENT;
  rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
  rej.reason = reason;
  ASC_rejectAssociation( assoc, &rej );
}

// ----------------------------------------------------------------------------

void DQDBaseSCPPool::dropAndDestroyAssociation(T_ASC_Association *assoc)
{
  if (assoc)
  {
    ASC_dropAssociation( assoc );
    ASC_destroyAssociation( &assoc );
  }
  assoc = NULL;
}

// ----------------------------------------------------------------------------

DcmSCPConfig& DQDBaseSCPPool::getConfig()
{
  return m_cfg;
}

// ----------------------------------------------------------------------------

void DQDBaseSCPPool::notifyThreadExit(DQDBaseSCPPool::DQDBaseSCPWorker* thread,
                                      OFCondition result)
{
  m_criticalSection.lock();
  if( m_runMode != SHUTDOWN )
  {
    DCMNET_DEBUG("DQDBaseSCPPool: Worker thread #" << thread->threadID() << " exited with error: " << result.text());
    m_workersBusy.remove(thread);
    delete thread;
    thread = NULL;
  }
  m_criticalSection.unlock();
}


/* *********************************************************************** */
/*                        DcmBaseSCPPool::BaseSCPWorker class              */
/* *********************************************************************** */

DQDBaseSCPPool::DQDBaseSCPWorker::DQDBaseSCPWorker(DQDBaseSCPPool& pool)
  : m_pool(pool),
    m_assoc(NULL)
{
}

// ----------------------------------------------------------------------------

DQDBaseSCPPool::DQDBaseSCPWorker::~DQDBaseSCPWorker()
{
  // do nothing
}

// ----------------------------------------------------------------------------

OFCondition DQDBaseSCPPool::DQDBaseSCPWorker::setAssociation(T_ASC_Association* assoc)
{
  if (busy())
    return NET_EC_AlreadyConnected;

  if ( (m_assoc != NULL) || (assoc == NULL) )
    return DIMSE_ILLEGALASSOCIATION;

  m_assoc = assoc;
  return EC_Normal;
}

// ----------------------------------------------------------------------------

void DQDBaseSCPPool::DQDBaseSCPWorker::run()
{
  OFCondition result;
  if(!m_assoc)
  {
    DCMNET_ERROR("DQDBaseSCPPool: Worker thread #" << threadID() << " received run command but has no association, exiting");
    m_pool.notifyThreadExit(this, ASC_NULLKEY);
    thread_exit();
  }
  else
  {
    T_ASC_Association *param = m_assoc;
    m_assoc = NULL;
    result = workerListen(param);
    DCMNET_DEBUG("DQDBaseSCPPool: Worker thread #" << threadID() << " returns with code: " << result.text() );
  }
  thread_exit();
  return;
}

// ----------------------------------------------------------------------------

void DQDBaseSCPPool::DQDBaseSCPWorker::exit()
{
  thread_exit();
}

#endif // WITH_THREADS
