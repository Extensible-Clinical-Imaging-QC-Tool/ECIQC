#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */ 
#include "dcmtk/dcmnet/diutil.h" 
#include "catch.hpp"

#ifdef WITH_THREADS

#include "../src/misc/Exception.hpp"
#include "../src/misc/MyLibrary.hpp"

#include "../src/communication/sender.hpp"
#include "../src/communication/receiver.hpp"

#define OFFIS_CONSOLE_APPLICATION "testscu" 

using namespace cpp_template;

static OFLogger echoscuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION); 

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v" 
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $"; 

// our application entity title used for calling the peer machine 
#define APPLICATIONTITLE     "TEST-SCU" 

// host name of the peer machine 
#define PEERHOSTNAME         "www.dicomserver.co.uk" 

// TCP/IP port to connect to the peer machine 
#define PEERPORT 11112 

// application entity title of the peer machine 
#define PEERAPPLICATIONTITLE "MOVESCP" 


TEST_CASE("Test C-ECHO Request with SCU","[ST]"){
  /* Setup DICOM connection parameters */ 
  OFLog::configure(OFLogger::DEBUG_LOG_LEVEL); 
  Sender scu(APPLICATIONTITLE, PEERHOSTNAME,PEERPORT, PEERAPPLICATIONTITLE); 
  // set AE titles 
  scu.setAETitle(APPLICATIONTITLE); 
  scu.setPeerHostName(PEERHOSTNAME); 
  scu.setPeerPort(PEERPORT); 
  scu.setPeerAETitle(PEERAPPLICATIONTITLE); 
  // Use presentation context for FIND/MOVE in study root, propose all uncompressed transfer syntaxes 
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

TEST_CASE("Test C-STORE Association with SCU","[ST]"){
  OFshared_ptr<OFList<DcmDataset>>  pt(new OFList<DcmDataset>);
  Receiver pool(PEERPORT, PEERAPPLICATIONTITLE);
  pool.setpointer(pt);

    /* Setup DICOM connection parameters */ 
  OFLog::configure(OFLogger::DEBUG_LOG_LEVEL); 
  Sender scu(APPLICATIONTITLE, PEERHOSTNAME,PEERPORT, PEERAPPLICATIONTITLE); 
   
  // Define presentation contexts, propose all uncompressed transfer syntaxes 
  OFList<OFString> ts; 
  ts.push_back(UID_LittleEndianExplicitTransferSyntax); 
  ts.push_back(UID_BigEndianExplicitTransferSyntax); 
  ts.push_back(UID_LittleEndianImplicitTransferSyntax); 

  // Define a separate transfer syntax needed for the X-ray image
  OFList<OFString> xfer;
  xfer.push_back(UID_LittleEndianImplicitTransferSyntax);

  // configure SCU 
  scu.setAETitle(APPLICATIONTITLE); 
  scu.setPeerHostName(PEERHOSTNAME); 
  scu.setPeerPort(PEERPORT); 
  scu.setPeerAETitle(PEERAPPLICATIONTITLE);
  scu.addPresentationContext(UID_CTImageStorage, ts); 
  scu.addPresentationContext(UID_MRImageStorage, ts); 
  scu.addPresentationContext(UID_DigitalXRayImageStorageForPresentation, xfer);
  scu.addPresentationContext(UID_VerificationSOPClass, ts); 

  //Start listening
  pool.start();

  /* Initialize network */ 
  OFCondition result = scu.initNetwork(); 
  CHECK(result.good()); 

  OFStandard::sleep(5);

  OFCondition status = scu.addDicomFile("../DICOM_Images/1-1copy.dcm", ERM_fileOnly,false);
  CHECK(status.good());

  /* Negotiate Association */ 
  result = scu.negotiateAssociation(); 
  CHECK(result.good());

  /*Assemble and send C-STORE request. Check if C-STORE was successful.*/
  Uint16 rspStatusCode = 0;
  result = scu.sendSTORERequest(0, "../DICOM_Images/1-1copy.dcm",0, rspStatusCode = 0);
  CHECK(result.good());

  /*Release association. */
  result = scu.releaseAssociation();
  CHECK(result.good());

  /*Request shutdown. */
  pool.request_stop();
  pool.join();

}

#endif



