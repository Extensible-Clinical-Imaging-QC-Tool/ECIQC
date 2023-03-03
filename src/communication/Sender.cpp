/**
 * @file test_scu.cpp
 * @brief Example code on how to program an actual DICOM client with the DcmStoreSCU class. 
 * @version 0.1
 * @date 2022-03-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */ 
#include "Sender.hpp"
#include "dcmtk/dcmnet/diutil.h" 

// Add the logging
#include "dcmtk/oflog/oflog.h"
#include "logging.hpp"

/**Constructor.*/

Sender::Sender():
  Sender("SCU", "localhost", 11112, "SCP")
{}

Sender::Sender(std::string aetitle, std::string peer_hostname, Uint16 peer_port, std::string peer_aetitle){
    
    //Methods to set SCU parameters.
    setAETitle(aetitle.c_str()); 
    setPeerHostName(peer_hostname.c_str()); 
    setPeerPort(peer_port); 
    setPeerAETitle(peer_aetitle.c_str());
    // Note: the following logger is wrong because it will be called during initialisation.
    /*
    OFLOG_INFO(get_logger(),"The sender starts to work" << '\n' 
        << "aetitle is: " << aetitle.c_str() << '\n' 
        << "Peer Host Name is: " << peer_hostname.c_str() << '\n'
        << "Peer port is: " << peer_port << '\n'
        << "Peer aetitle is: " << peer_aetitle.c_str() << std::endl);
    */

    
    //Define presentation contexts, propose all uncompressed TS
    /* 
    OFList<OFString> ts;
    ts.push_back(UID_LittleEndianExplicitTransferSyntax); 
    ts.push_back(UID_BigEndianExplicitTransferSyntax); 
    ts.push_back(UID_LittleEndianImplicitTransferSyntax); 
    addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, ts); 
    addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, ts); 
    addPresentationContext(UID_VerificationSOPClass, ts);

    // Define a separate transfer syntax needed for the X-ray image
    OFList<OFString> xfer;
    xfer.push_back(UID_LittleEndianImplicitTransferSyntax);
    //Define a separate transfer syntax needed for multiframe US image
    OFList<OFString> xfer2;
    xfer2.push_back(UID_JPEGProcess1TransferSyntax);
    
    //Configure SCU to include more PCs for other SOPs
    addPresentationContext(UID_CTImageStorage, ts); 
    addPresentationContext(UID_MRImageStorage, ts); 
    addPresentationContext(UID_DigitalXRayImageStorageForPresentation, xfer); 
    addPresentationContext(UID_UltrasoundMultiframeImageStorage, xfer2);
    */
   /*
   OFList<OFString> xfers;
   xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
   xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
   xfers.push_back(UID_JPEGProcess14SV1TransferSyntax);
   xfers.push_back(UID_JPEGProcess1TransferSyntax);

   // Define a separate transfer syntax needed for the X-ray image
   OFList<OFString> ts;
   ts.push_back(UID_LittleEndianImplicitTransferSyntax);
   //ts.push_back(UID_JPEGProcess14SV1TransferSyntax);
   //ts.push_back(UID_JPEGProcess1TransferSyntax);

   addPresentationContext(UID_CTImageStorage, xfers);
   addPresentationContext(UID_MRImageStorage, xfers);
   addPresentationContext(UID_UltrasoundMultiframeImageStorage,ts);
   addPresentationContext(UID_UltrasoundMultiframeImageStorage,xfers);
   addPresentationContext(UID_SecondaryCaptureImageStorage,ts);
   addPresentationContext(UID_SecondaryCaptureImageStorage,xfers);
  
   addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts);
   addPresentationContext(UID_VerificationSOPClass, xfers);
   */
   // Add presentation context to be handled
   /*
   OFList<OFString> ts;
   ts.push_back(UID_LittleEndianExplicitTransferSyntax);
   ts.push_back(UID_LittleEndianImplicitTransferSyntax);
   ts.push_back(UID_BigEndianExplicitTransferSyntax);
   ts.push_back(UID_JPEGProcess14SV1TransferSyntax);
   ts.push_back(UID_JPEGProcess1TransferSyntax);
   for (size_t n = 0; n < (size_t)numberOfDcmLongSCUStorageSOPClassUIDs; n++) {
       addPresentationContext(dcmLongSCUStorageSOPClassUIDs[n], ts);
   }
   addPresentationContext(UID_VerificationSOPClass, ts);
   */

   // Tried by Yiming 2023-03-03, just add one specific syntax according to the files
   OFList<OFString> ts1;
   OFList<OFString> ts2;
   ts1.push_back(UID_LittleEndianImplicitTransferSyntax);
   ts2.push_back(UID_JPEGProcess1TransferSyntax);
   addPresentationContext(UID_CTImageStorage, ts1);
   addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts1);
   addPresentationContext(UID_SecondaryCaptureImageStorage,ts1);
   addPresentationContext(UID_MRImageStorage, ts1);
   addPresentationContext(UID_VerificationSOPClass, ts1);
   addPresentationContext(UID_UltrasoundMultiframeImageStorage,ts2);
   
}

void Sender::set_aetitle(const std::string& title) {
    setAETitle(title.c_str()); 
}
void Sender::set_peer_port(int port) {
  setPeerPort(port);
}
void Sender::set_peer_hostname(const std::string& hostname) {
  setPeerHostName(hostname.c_str());
}
void Sender::set_peer_aetitle(const std::string& title) {
  setPeerAETitle(title.c_str());
}


OFCondition Sender::send(DcmDataset& dataset) {
  auto result = initNetwork();
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in initNetwork!" << std::endl);
      return result;
  }
  result = negotiateAssociation(); 
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in negotiate association!" << std::endl);
      return result;
  }
  result = addDataset(&dataset);
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in adding dataset!" << std::endl);
      return result;
  }
  result = sendSOPInstances();
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in sending SOP instances!" << std::endl);
      return result;
  }
  Uint16 rspStatusCode = 0;
  result = sendSTORERequest(0, nullptr, &dataset, rspStatusCode);
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in sending STORE Request!" << std::endl);
      return result;
  }
  return releaseAssociation();
}

OFCondition Sender::send_file(const std::string& filename) {
  auto result = initNetwork();
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in initNetwork!" << std::endl);
      return result;
  }
  result = negotiateAssociation(); 
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in negotiate association!" << std::endl);
      return result;
  }
  result = addDicomFile(filename.c_str(), ERM_fileOnly, false);
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in adding Dicom File!" << std::endl);
      return result;
  }
  result = sendSOPInstances();
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in sending SOP instances!" << std::endl);
      return result;
  }
  Uint16 rspStatusCode = 0;
  result = sendSTORERequest(0, filename.c_str(), nullptr, rspStatusCode);
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in sending STORE Request!" << std::endl);
      return result;
  }
  return releaseAssociation();
}

OFCondition Sender::send_echo() {
  auto result = initNetwork();
  result = negotiateAssociation(); 
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in negotiate association!" << std::endl);
      return result;
  }
  result = sendECHORequest(0);
  if (result.bad()) {
      OFLOG_ERROR(get_logger(),"unsuccessful in sending ECHO Request!" << std::endl);
      return result;
  }
  return releaseAssociation();
}



// ---------------------------------------------------------------------------


