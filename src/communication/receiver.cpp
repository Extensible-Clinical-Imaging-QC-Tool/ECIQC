#include <iostream>
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmpstat/dcmpstat.h>

#include "receiver.hpp"

// ----------------------------------------------------------------------------

ReceiverThread::ReceiverThread():DcmThreadSCP()
{
 
}

// ----------------------------------------------------------------------------

ReceiverThread::~ReceiverThread() {}

// ----------------------------------------------------------------------------

void ReceiverThread::setdatasetaddress(OFshared_ptr<OFList<DcmDataset>>dset){
   m_dset = dset;
}

// ----------------------------------------------------------------------------

OFCondition ReceiverThread::handleIncomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo)
    {

        if (incomingMsg->CommandField == DIMSE_C_STORE_RQ)
        {
            
            
            OFList<DcmDataset>* pt = m_dset.get();
            
            DcmDataset dset;
            DcmDataset *reqdataset = &dset;
            OFCondition result = DcmSCP::handleSTORERequest(incomingMsg->msg.CStoreRQ, presInfo.presentationContextID, reqdataset);
            
            pt->push_back(dset);

            
        
            return result;
            
            
        }

        else
        {
            return DcmSCP::handleIncomingCommand(incomingMsg, presInfo);
        }
    }

// ----------------------------------------------------------------------------

OFBool ReceiverThread::checkCallingHostAccepted(const OFString& hostOrIP)
{
    // Check if acceptable IPs/hostnames have been specified. 
    if(m_sourcelist.size()!= 0) {
        
        // Check if peer's hostname is in the acceptable source list.
        OFListIterator(OFString) it = m_sourcelist.begin();
        OFListIterator(OFString) last = m_sourcelist.end();
        
        while(it != last)
        {
            OFString item = *it;
            if (item == getPeerIP())
            {
                return OFTrue;
                it = last;
            }
            ++it;
        }
        return OFFalse;
    }
    else
    {
        return OFTrue;
    }
    

}

// ----------------------------------------------------------------------------

OFBool ReceiverThread::checkCallingAETitleAccepted(const OFString& callingAE)
{
    if(m_peerAETitles.size() != 0)
    {
        OFListIterator(OFString) it = m_peerAETitles.begin();
        OFListIterator(OFString) last = m_peerAETitles.end();

        while(it != last)
        {
            OFString item = *it;
            if (item == getPeerAETitle())
            {
                return OFTrue;
                it = last;
            }
            ++it;
        }
        
        return OFFalse;
    }
    else
    {
        return OFTrue;
    }
}

// ----------------------------------------------------------------------------

OFCondition ReceiverThread::setIPs(const OFList<OFString>& source_list)
{
    if (isConnected())
  {
    return EC_IllegalCall; // TODO: need to find better error code
  }
  m_sourcelist = source_list;
  return EC_Normal;
}

// ----------------------------------------------------------------------------

OFCondition ReceiverThread::setpeerAETitles(const OFList<OFString>& peerae_list){
     if (isConnected())
  {
    return EC_IllegalCall; // TODO: need to find better error code
  }
  m_peerAETitles = peerae_list;
  return EC_Normal;
}

/* *********************************************************************** */
/*                        Receiver class                                   */
/* *********************************************************************** */

Receiver::Receiver(Uint16 port, std::string aetitle)
{       OFString title= OFString(aetitle.c_str());
        // Configure SCP port
        getConfig().setPort(port);
        // Configure SCP name
        getConfig().setAETitle(title);
        // Set number of threads
        setMaxThreads(2);
        getConfig().setConnectionBlockingMode(DUL_NOBLOCK);

        // Set time to wait for the next association (in seconds)
        getConfig().setConnectionTimeout(5);
        // Set verbose mode
        getConfig().setVerbosePCMode(OFTrue);
        


        // Add presentation context to be handled
        OFList<OFString> ts;
        ts.push_back(UID_LittleEndianExplicitTransferSyntax);
        ts.push_back(UID_LittleEndianImplicitTransferSyntax);
        ts.push_back(UID_BigEndianExplicitTransferSyntax);
        ts.push_back(UID_JPEGProcess14SV1TransferSyntax);
        ts.push_back(UID_JPEGProcess1TransferSyntax);
        for (size_t n = 0; n < (size_t) numberOfDcmLongSCUStorageSOPClassUIDs; n++)
        {
            getConfig().addPresentationContext(dcmLongSCUStorageSOPClassUIDs[n], ts);
        }
        getConfig().addPresentationContext(UID_VerificationSOPClass, ts);
    }

// ----------------------------------------------------------------------------

Receiver::~Receiver() {}

// ----------------------------------------------------------------------------

void Receiver::request_stop(){ DQDBaseSCPPool::stopAfterCurrentAssociations(); }

// ----------------------------------------------------------------------------

void Receiver::run()
    {
        result = DQDSCPPool :: listen();
    }

// ----------------------------------------------------------------------------

void Receiver::setaetitle(std::string ae_title) 
{
    getConfig().setAETitle(ae_title.c_str());
}

// ----------------------------------------------------------------------------

void Receiver::setportnumber(Uint16 port)
{
    getConfig().setPort(port);
}

