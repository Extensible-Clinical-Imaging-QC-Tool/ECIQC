#include <limits>
#include <type_traits>
#include <iostream>
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmpstat/dcmpstat.h>

#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftimer.h"

#include <dcmtk/dcmnet/scu.h>
#include "dcmtk/dcmnet/scp.h"
#include <dcmtk/dcmnet/dstorscu.h>
#include "dcmtk/dcmnet/scp.h"
#include "dcmtk/dcmnet/dstorscp.h"

#include "dcmtk/dcmnet/diutil.h"

#include "../src/Exception.hpp"
#include "../src/MyLibrary.hpp"

#include "../src/Sender_draft1.hpp"
#include "../src/draft_receiver.hpp"

using namespace cpp_template;

/* Set parameters */

//our application entity title used for calling the peer machine 
#define APPLICATIONTITLE     "TEST-SCU" 

// host name of the peer machine 
#define PEERHOSTNAME         "www.dicomserver.co.uk" 

// TCP/IP port to connect to the peer machine 
#define PEERPORT 11112 

// application entity title of the peer machine 
#define PEERAPPLICATIONTITLE "MOVESCP" 

// MOVE destination AE Title 
#define MOVEAPPLICATIONTITLE "TEST-SCU" 


/* Setup DICOM connection parameters */ 

DcmStorageSCU scu;
OFCondition result;


TEST_CASE("Test the Sender Class parameters"){

  std::string s = std::to_string(PEERPORT); 
  const char* opt_peer = s.c_str(); 
  const char* opt_peerTitle = PEERAPPLICATIONTITLE;
  const char* dicom_dir = "../DICOM_Images/";
  OFCmdUnsignedInt opt_port = 0;


  Sender object;

  CHECK(object.Send_Dicom_Files(opt_peer,opt_peerTitle,dicom_dir));

}

TEST_CASE("Test the Sender Class inputs and C-ECHO request"){



  std::string s = std::to_string(PEERPORT); 
  const char* opt_peer = s.c_str(); 
  const char* opt_peerTitle = PEERAPPLICATIONTITLE;
  const char* dicom_dir = "../DICOM_Images/";
  OFCmdUnsignedInt opt_port = 0;


  Sender object;


  /* check Sender class functions */
  scu.setReadFromDICOMDIRMode(dicom_dir);
  CHECK(result.good());

  int paramCount = 3; // number of params 

  /* Iterate over input filenames */
  OFList<OFString> inputFiles; 

  for (int i = 3; i <= paramCount; i++){
      if (OFStandard::dirExists(dicom_dir)){
          OFStandard::searchDirectoryRecursively(dicom_dir,inputFiles);
      }
      else{
          inputFiles.push_back(dicom_dir);
      }
  }
  
  /* check files do exist in the directory */
  CHECK_FALSE(inputFiles.empty());

  scu.setReadFromDICOMDIRMode(dicom_dir);
  scu.setHaltOnInvalidFileMode(OFTrue);



  /* check iterating over all input filenames works and stored */
  OFListIterator(OFString) iter = inputFiles.begin();
  OFListIterator(OFString) last = inputFiles.end();
  OFString filename; 
  while (iter != last){
    filename = (*iter).c_str();
    result = scu.addDicomFile(filename);
    CHECK(result.good());
    ++iter;
  }
  /* check the input files are valid */
  CHECK_FALSE(scu.getNumberOfSOPInstances() == 0);


  /* set network parameters */
  scu.setAETitle(APPLICATIONTITLE); 
  scu.setPeerHostName(opt_peer);
  scu.setPeerPort(OFstatic_cast(Uint16, opt_port));
  scu.setPeerAETitle(opt_peerTitle);


  /* Use presentation context for FIND/MOVE in study root */
  OFList<OFString> ts; 
  ts.push_back(UID_LittleEndianExplicitTransferSyntax); 
  ts.push_back(UID_BigEndianExplicitTransferSyntax); 
  ts.push_back(UID_LittleEndianImplicitTransferSyntax); 
  scu.addPresentationContext(UID_VerificationSOPClass, ts);// C-ECHO operation

  /* Add presentation contexts to be negotiated */
  result = scu.addPresentationContext(UID_VerificationSOPClass, ts); 
  CHECK(result.good());

	scu.addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, ts); // C-FIND operation
	scu.addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, ts); // C-MOVE operation




  /* Implement a test Receiver */
  DcmStorageSCP scp; // Create a DcmStorageSCP object
	scp.setAETitle(MOVEAPPLICATIONTITLE); // Set AET to the AET specified by the C-MOVE operation
	scp.setPort(1235); // Set the port number that storeSCP listens to
  scp.setVerbosePCMode(OFTrue); // Set Verbose mode for debugging
	scp.addPresentationContext(UID_VerificationSOPClass, ts); // Set the abstract syntax and transfer syntax of the receiver
	scp.setOutputDirectory("../DICOM_Images/DICOM_MOVE/"); // Set the output directory 

  // To do : handle incoming command and send response 
  
  
  Receiver pool;
  pool.start();


  /* Initialize network */
  result = scu.initNetwork();
  CHECK(result.good());

  /* Negotiate network association with peer */
  //result = scu.negotiateAssociation();
  //CHECK(result.good());

  

  /* Check whether the server is listening: 
	Assemble and send C-ECHO request 
	*/ 
	//result = scu.sendECHORequest(0); 
  //CHECK(result.good());



  /* Check SOP instances to be transferred */
  //result = scu.sendSOPInstances();
  //CHECK(result.good());


  /* Request shutdown */
  pool.request_stop();
  pool.join();
  
  /* Close current association */
  result = scu.releaseAssociation();


  /* Check if report file can be created */
  const char *report_filename = "Sender_Logs";
  result = scu.createReportFile(report_filename);
  CHECK(result.good());


}


//#endif
