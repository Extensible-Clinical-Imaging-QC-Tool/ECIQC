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
Sender::Sender(std::string SenderAETitle, std::string ReceiverPortName, Uint16 ReceiverPortNumber, std::string ReceiverAETitle){
    
    //Methods to set SCU parameters.
    setAETitle(SenderAETitle.c_str()); 
    setPeerHostName(ReceiverPortName.c_str()); 
    setPeerPort(ReceiverPortNumber); 
    setPeerAETitle(ReceiverAETitle.c_str());

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


OFCondition Sender::send(DcmDataset& dataset) {
  auto result = negotiateAssociation(); 
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
  return releaseAssociation();
}



// ---------------------------------------------------------------------------


