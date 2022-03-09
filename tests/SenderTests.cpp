#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */ 
#include "dcmtk/dcmnet/diutil.h" 
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/oflog/fileap.h"
#include "catch.hpp"

#include "../src/misc/Exception.hpp"
#include "../src/misc/MyLibrary.hpp"

#include "../src/communication/sender.hpp"
#include "../src/communication/receiver.hpp"

using namespace cpp_template;

#define OFFIS_CONSOLE_APPLICATION "my_Logger"

static OFLogger my_Logger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


    
    

TEST_CASE("Test C-ECHO Request with SCU","[ST]"){

  /* specify log pattern */
    OFunique_ptr<dcmtk::log4cplus::Layout> layout(new dcmtk::log4cplus::PatternLayout("%D{%Y-%m-%d %H:%M:%S.%q} %5p: %m%n"));
    /* Denote that a log file should be used that is appended to. The file is re-created every
       time the code gets to this point.
     */
    dcmtk::log4cplus::SharedAppenderPtr logfile(new dcmtk::log4cplus::FileAppender("ST.log"));
    //logfile->setLayout(OFmove(layout));

    /* make sure that only the file logger is used */
    dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
    log.removeAllAppenders();
    log.addAppender(logfile);
    log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL);

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

TEST_CASE("Test Unsuccessful C-STORE Association with SCU","[STS]"){

  /* specify log pattern */
    OFunique_ptr<dcmtk::log4cplus::Layout> layout(new dcmtk::log4cplus::PatternLayout("%D{%Y-%m-%d %H:%M:%S.%q} %5p: %m%n"));
    /* Denote that a log file should be used that is appended to. The file is re-created every
       time the code gets to this point.
     */
    dcmtk::log4cplus::SharedAppenderPtr logfile(new dcmtk::log4cplus::FileAppender("STS.log"));
    //logfile->setLayout(OFmove(layout));

    /* make sure that only the file logger is used */
    dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
    log.removeAllAppenders();
    log.addAppender(logfile);
    log.setLogLevel(OFLogger::INFO_LOG_LEVEL);
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

  OFCondition status = scu.addDicomFile("../DICOM_Images/testtext.dcm", ERM_fileOnly,false);
  CHECK(status.good());

  /*Extracting data from dicom file.*/ 
  DcmFileFormat dfile;
  result = dfile.loadFile("../DICOM_Images/testtext.dcm");
  CHECK (result.good());
  DcmDataset *data = dfile.getDataset();
  CHECK(data!= NULL);

  /* Negotiate Association */ 
  result = scu.negotiateAssociation(); 
  CHECK(result.good());

  /*Assemble and send C-STORE request. Check if C-STORE was successful.*/
  Uint16 rspStatusCode = 0;
  result = scu.sendSTORERequest(0, /*"../DICOM_Images/1-01.dcm"*/ 0,/*0*/data, rspStatusCode = 0);
  CHECK(result.bad());
  if (result.bad()){   
    status = data->saveFile("../DICOM_Images/Archive_1.dcm");
    CHECK(status.good());
    }
    

  /*Release association. */
  result = scu.releaseAssociation();
  CHECK(result.good());

  /*Request shutdown and stop listening. */ 
  pool.request_stop();
  pool.join();
  

}

TEST_CASE("Test Successful C-STORE Association with SCU","[STS2]"){
  /* specify log pattern */
    OFunique_ptr<dcmtk::log4cplus::Layout> layout(new dcmtk::log4cplus::PatternLayout("%D{%Y-%m-%d %H:%M:%S.%q} %5p: %m%n"));
    /* Denote that a log file should be used that is appended to. The file is re-created every
       time the code gets to this point.
     */
    dcmtk::log4cplus::SharedAppenderPtr logfile(new dcmtk::log4cplus::FileAppender("STS2.log"));
    //logfile->setLayout(OFmove(layout));

    /* make sure that only the file logger is used */
    dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
    log.removeAllAppenders();
    log.addAppender(logfile);
    log.setLogLevel(OFLogger::INFO_LOG_LEVEL);
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
  result = dfile.loadFile("../DICOM_Images/1-01.dcm");
  CHECK (result.good());
  DcmDataset *data = dfile.getDataset();
  CHECK(data!= NULL);

  /* Negotiate Association */ 
  result = scu.negotiateAssociation(); 
  CHECK(result.good());

  /*Assemble and send C-STORE request. Check if C-STORE was successful.*/
  Uint16 rspStatusCode = 0;
  result = scu.sendSTORERequest(0, /*"../DICOM_Images/1-01.dcm"*/ 0,/*0*/data, rspStatusCode = 0);
  CHECK(result.good());
  if (result.bad()){
    status = data->saveFile("../DICOM_Images/Archive_1.dcm");
    CHECK(status.good());
  }
    

  /*Release association. */
  result = scu.releaseAssociation();
  CHECK(result.good());

  /*Request shutdown and stop listening. */ 
  pool.request_stop();
  pool.join();
  

}





