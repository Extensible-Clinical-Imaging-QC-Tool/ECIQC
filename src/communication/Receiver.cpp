#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <iostream>

#include "communication/ThreadSafeQueue.hpp"
#include "Receiver.hpp"

// Add the logging module
#include "dcmtk/oflog/oflog.h"
#include "logging.hpp"

#include "../libs/nlohmann_json/single_include/nlohmann/json.hpp"
using json = nlohmann::ordered_json;

// ----------------------------------------------------------------------------

ReceiverThread::ReceiverThread() : DcmThreadSCP() {}

// ----------------------------------------------------------------------------

ReceiverThread::~ReceiverThread() {}

// ----------------------------------------------------------------------------

void ReceiverThread::setdatasetaddress(
    OFshared_ptr<ThreadSafeQueue<DcmDataset>> dset) {
  m_dset = dset;
}

// ----------------------------------------------------------------------------

OFCondition ReceiverThread::handleIncomingCommand(
    T_DIMSE_Message *incomingMsg, const DcmPresentationContextInfo &presInfo) {

  if (incomingMsg->CommandField == DIMSE_C_STORE_RQ) {

    ThreadSafeQueue<DcmDataset> *pqueue = m_dset.get();

    DcmDataset dset;
    DcmDataset *reqdataset = &dset;
    OFCondition result = DcmSCP::handleSTORERequest(
        incomingMsg->msg.CStoreRQ, presInfo.presentationContextID, reqdataset);

    std::cout << "adding dataset to receiver queue" << std::endl;
    pqueue->push(std::move(dset));

    return result;

  }

  else {
    return DcmSCP::handleIncomingCommand(incomingMsg, presInfo);
  }
}

// ----------------------------------------------------------------------------

OFBool ReceiverThread::checkCallingHostAccepted(const OFString &hostOrIP) {
  // Check if acceptable IPs/hostnames have been specified.
  if (m_sourcelist.size() != 0) {

    // Check if peer's hostname is in the acceptable source list.
    OFListIterator(OFString) it = m_sourcelist.begin();
    OFListIterator(OFString) last = m_sourcelist.end();

    while (it != last) {
      OFString item = *it;
      if (item == getPeerIP()) {
        return OFTrue;
        it = last;
      }
      ++it;
    }
    return OFFalse;
  } else {
    return OFTrue;
  }
}

// ----------------------------------------------------------------------------

OFBool ReceiverThread::checkCallingAETitleAccepted(const OFString &callingAE) {
  if (m_peerAETitles.size() != 0) {
    OFListIterator(OFString) it = m_peerAETitles.begin();
    OFListIterator(OFString) last = m_peerAETitles.end();

    while (it != last) {
      OFString item = *it;
      if (item == getPeerAETitle()) {
        return OFTrue;
        it = last;
      }
      ++it;
    }

    return OFFalse;
  } else {
    return OFTrue;
  }
}

// ----------------------------------------------------------------------------

OFCondition ReceiverThread::setIPs(const OFList<OFString> &source_list) {
  if (isConnected()) {
    return EC_IllegalCall; // TODO: need to find better error code
  }
  m_sourcelist = source_list;
  return EC_Normal;
}

// ----------------------------------------------------------------------------

OFCondition
ReceiverThread::setpeerAETitles(const OFList<OFString> &peerae_list) {
  if (isConnected()) {
    return EC_IllegalCall; // TODO: need to find better error code
  }
  m_peerAETitles = peerae_list;
  return EC_Normal;
}

/* *********************************************************************** */
/*                        Receiver class                                   */
/* *********************************************************************** */

Receiver::Receiver() : Receiver(11112, "SCP"){};

Receiver::Receiver(Uint16 port, std::string aetitle) {
  OFString title = OFString(aetitle.c_str());
  // Configure SCP port
  getConfig().setPort(port);
  // Configure SCP name
  getConfig().setAETitle(title);

  // Add logging
  OFLOG_INFO(get_logger(),"Receiver starts to work!" << '\n'
            <<" SCP Port: " << port << '\n'
            <<" title: " << title << std::endl);
  // Set number of threads
  setMaxThreads(2);
  getConfig().setConnectionBlockingMode(DUL_NOBLOCK);

  // Set time to wait for the next association (in seconds)
  getConfig().setConnectionTimeout(5);
  // Set verbose mode
  getConfig().setVerbosePCMode(OFTrue);

  OFList<OFString> xfers;
   xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
   xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

   // Define a separate transfer syntax needed for the X-ray image
   OFList<OFString> ts;
   ts.push_back(UID_LittleEndianImplicitTransferSyntax);

   OFList<OFString> ts2;
   ts2.push_back(UID_JPEGProcess1TransferSyntax);

   getConfig().addPresentationContext(UID_CTImageStorage, xfers);
   getConfig().addPresentationContext(UID_MRImageStorage, xfers);
   getConfig().addPresentationContext(UID_SecondaryCaptureImageStorage,xfers);
   getConfig().addPresentationContext(UID_VerificationSOPClass, xfers);
   getConfig().addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts);
   getConfig().addPresentationContext(UID_UltrasoundMultiframeImageStorage,ts2);
}

// ----------------------------------------------------------------------------

void Receiver::request_stop() {
  DQDBaseSCPPool::stopAfterCurrentAssociations();
  OFLOG_INFO(get_logger(),"Request stop of the receiver" << std::endl);
}

// ----------------------------------------------------------------------------

void Receiver::run() { 
    result = DQDSCPPool ::listen();
    OFLOG_INFO(get_logger(),"Receiver is running now!" << std::endl);
    }

// ----------------------------------------------------------------------------

void Receiver::setaetitle(std::string ae_title) {
  getConfig().setAETitle(ae_title.c_str());
}

// ----------------------------------------------------------------------------

void Receiver::setportnumber(Uint16 port) { getConfig().setPort(port); }

void Receiver::set_additional_context(const json &context){
    OFList<OFString> xfers;
    for (const auto &param : context.items()) {
        xfers.clear();
        OFString UID = OFString(param.key().c_str());
        const auto &ts_value = param.value(); 
        
        for (const auto &xfer : ts_value.get<std::vector<std::string>>())
        {
          xfers.push_back(xfer.c_str());
        }
        
        getConfig().addPresentationContext(UID, xfers);
    }
    
}
