#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */ 
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmnet/diutil.h" 
#include "catch.hpp"

#include "../src/misc/Exception.hpp"
#include "../src/misc/MyLibrary.hpp"

#include "../src/communication/sender.hpp"
#include "../src/communication/receiver.hpp"

using namespace cpp_template;

TEST_CASE("Test C-ECHO Request with SCU","[ST]"){
  /* Setup DICOM connection parameters */ 
  OFString ae_title = "TEST-SCU";
  OFString peer_hostname = "www.dicomserver.co.uk";
  Uint16 peer_port = 11112;
  OFString peer_aetitle = "MOVESCP";
  Sender scu(ae_title, peer_hostname, peer_port, peer_aetitle); 
  // set AE titles 
  scu.setAETitle(ae_title); 
  scu.setPeerHostName(peer_hostname); 
  scu.setPeerPort(peer_port); 
  scu.setPeerAETitle(peer_aetitle); 
  // Define presentation contexts, propose all uncompressed transfer syntaxes 
  OFList<OFString> ts; 
  ts.push_back(UID_LittleEndianExplicitTransferSyntax); 
  ts.push_back(UID_BigEndianExplicitTransferSyntax); 
  ts.push_back(UID_LittleEndianImplicitTransferSyntax); 
  scu.addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, ts); 
  scu.addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, ts); 
  scu.addPresentationContext(UID_VerificationSOPClass, ts); 
  /* Initialize network */ 
  OFCondition result = scu.initNetwork(); 
  CHECK(result.good()); 


  /* Negotiate Association */ 
  result = scu.negotiateAssociation(); 
  CHECK(result.good());

  /* Let's look whether the server is listening: 
     Assemble and send C-ECHO request 
   */ 
  result = scu.sendECHORequest(0); 
  CHECK(result.good());

  /* Release association */ 
  result = scu.releaseAssociation();
  CHECK(result.good());


}

TEST_CASE("Test C-STORE Association with SCU","[STS]"){
  OFshared_ptr<OFList<DcmDataset>>  pt(new OFList<DcmDataset>);
  Receiver pool(104, "MOVESCP");
  pool.setpointer(pt);
  

    /* Setup DICOM connection parameters */
  OFString ae_title = "TEST-SCU";/*"StoreTestSCU";*/
  OFString peer_hostname = "www.dicomserver.co.uk";
  Uint16 peer_port = 104;
  OFString peer_aetitle = "MOVESCP";/*"TestSCP";*/
  Sender scu(ae_title, peer_hostname, peer_port, peer_aetitle);  
   
   
  // Define presentation contexts, propose all uncompressed transfer syntaxes 
  OFList<OFString> ts; 
  ts.push_back(UID_LittleEndianExplicitTransferSyntax); 
  ts.push_back(UID_BigEndianExplicitTransferSyntax); 
  ts.push_back(UID_LittleEndianImplicitTransferSyntax); 

  // Define a separate transfer syntax needed for the X-ray image
  OFList<OFString> xfer;
  xfer.push_back(UID_LittleEndianImplicitTransferSyntax);

  //Start listening
  pool.start();

  // configure SCU 
  scu.setAETitle(ae_title); 
  scu.setPeerHostName(peer_hostname); 
  scu.setPeerPort(peer_port); 
  scu.setPeerAETitle(peer_aetitle);
  scu.setVerbosePCMode(OFTrue);
  scu.addPresentationContext(UID_CTImageStorage, ts); 
  scu.addPresentationContext(UID_MRImageStorage, ts); 
  scu.addPresentationContext(UID_DigitalXRayImageStorageForPresentation, xfer);
  scu.addPresentationContext(UID_VerificationSOPClass, ts); 

  /* Initialize network */ 
  OFCondition result = scu.initNetwork(); 
  CHECK(result.good());

  OFCondition status = scu.addDicomFile("../DICOM_Images/1-01.dcm", ERM_fileOnly,false);
  CHECK(status.good());

  /*Extracting data from dicom file.*/ 
  DcmFileFormat dfile;
  result = dfile.loadFile("/home/hazelweeling/ECIQC/DICOM_Images/1-01.dcm");
  CHECK (result.good());
  DcmDataset *data = dfile.getDataset();

  /* Negotiate Association */ 
  result = scu.negotiateAssociation(); 
  CHECK(result.good());

  /*Assemble and send C-STORE request. Check if C-STORE was successful.*/
  Uint16 rspStatusCode = 0;
  result = scu.sendSTORERequest(0, /*"../DICOM_Images/1-01.dcm"*/ 0,/*0*/data, rspStatusCode = 0);
  CHECK(result.good());

  /*Release association. */
  result = scu.releaseAssociation();
  CHECK(result.good());

  /*Request shutdown and stop listening. */ 
  pool.request_stop();
  pool.join();
  

}





