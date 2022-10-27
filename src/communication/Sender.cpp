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
#include "sender.hpp"
#include "dcmtk/dcmnet/diutil.h" 

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

    //Define presentation contexts, propose all uncompressed TS
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
      return result;
  }
  result = negotiateAssociation(); 
  if (result.bad()) {
      return result;
  }
  result = addDataset(&dataset);
  if (result.bad()) {
      return result;
  }
  result = sendSOPInstances();
  if (result.bad()) {
      return result;
  }
  Uint16 rspStatusCode = 0;
  result = sendSTORERequest(0, nullptr, &dataset, rspStatusCode);
  if (result.bad()) {
      return result;
  }
  return releaseAssociation();
}

OFCondition Sender::send_file(const std::string& filename) {
  auto result = initNetwork();
  if (result.bad()) {
      return result;
  }
  result = negotiateAssociation(); 
  if (result.bad()) {
      return result;
  }
  result = addDicomFile(filename.c_str(), ERM_fileOnly, false);
  if (result.bad()) {
      return result;
  }
  result = sendSOPInstances();
  if (result.bad()) {
      return result;
  }
  Uint16 rspStatusCode = 0;
  result = sendSTORERequest(0, filename.c_str(), nullptr, rspStatusCode);
  if (result.bad()) {
      return result;
  }
  return releaseAssociation();
}

OFCondition Sender::send_echo() {
  auto result = initNetwork();
  result = negotiateAssociation(); 
  if (result.bad()) {
      return result;
  }
  result = sendECHORequest(0);
  if (result.bad()) {
      return result;
  }
  return releaseAssociation();
}



// ---------------------------------------------------------------------------

