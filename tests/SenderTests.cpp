#include <limits>
#include <type_traits>
#include <iostream>
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

//#ifdef WITH_THREADS

//#define INCLUDE_CMATH

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <dcmtk/dcmnet/scu.h>
#include "dcmtk/dcmnet/scp.h"
#include <dcmtk/dcmnet/dstorscu.h>

#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftimer.h"

#include "../src/Exception.hpp"
#include "../src/MyLibrary.hpp"

#include "../src/Sender.hpp"

using namespace cpp_template;

// our application entity title used for calling the peer machine 
#define APPLICATIONTITLE     "TEST-SCU" 

// host name of the peer machine 
#define PEERHOSTNAME         "www.dicomserver.co.uk" 

// TCP/IP port to connect to the peer machine 
#define PEERPORT 11112 

// application entity title of the peer machine 
#define PEERAPPLICATIONTITLE "MOVESCP" 

// MOVE destination AE Title 
#define MOVEAPPLICATIONTITLE "TEST-SCU" 

static Uint8 findUncompressedPC(const OFString& sopClass, 
                                DcmSCU& scu) 
{ 
  Uint8 pc; 
  pc = scu.findPresentationContextID(sopClass, UID_LittleEndianExplicitTransferSyntax); 
  if (pc == 0) 
    pc = scu.findPresentationContextID(sopClass, UID_BigEndianExplicitTransferSyntax); 
  if (pc == 0) 
    pc = scu.findPresentationContextID(sopClass, UID_LittleEndianImplicitTransferSyntax); 
  return pc; 
} 


/* Setup DICOM connection parameters */ 

DcmSCU scu;
OFCondition result;



TEST_CASE("Test network initialisation"){
  /* Initialize network */ 
  OFCondition result = scu.initNetwork(); 
  CHECK(result.good());
}

// This tests that a connection can be negtotiated with the peer host
TEST_CASE("Test network association with peer"){
  /* Negotiate Association */ 
  result = scu.negotiateAssociation(); 
  CHECK(result.good());
}
  

// This tests whether the server is listening
TEST_CASE("Test the send C-ECHO request"){
  /*Assemble and send C-ECHO request */
  result = scu.sendECHORequest(0); 
  CHECK(result.good());
}


TEST_CASE("Test the send C-FIND request"){
  /* Assemble and send C-FIND request */
  OFList<QRResponse*> findResponses;
  // Load DICOM files 
  DcmFileFormat fileformat;
  fileformat.loadFile("../DICOM_Images/1-1copy.dcm");
  DcmDataset *req = fileformat.getDataset();
  req->putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "STUDY"); 
  req->putAndInsertOFStringArray(DCM_StudyInstanceUID, ""); 
  T_ASC_PresentationContextID presID = findUncompressedPC(UID_FINDStudyRootQueryRetrieveInformationModel, scu);
  CHECK_FALSE(presID == 0);
  result = scu.sendFINDRequest(presID, req, &findResponses); 
  CHECK(result.good());

}
TEST_CASE("Test the send C-MOVE request"){
  OFCondition result = scu.initNetwork(); 
  OFList<QRResponse*> findResponses; 
  // Load DICOM files 
  DcmFileFormat fileformat;
  fileformat.loadFile("../DICOM_Images/1-1copy.dcm");
  DcmDataset *req = fileformat.getDataset();
  req->putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "STUDY"); 
  req->putAndInsertOFStringArray(DCM_StudyInstanceUID, ""); 
  T_ASC_PresentationContextID presID = findUncompressedPC(UID_FINDStudyRootQueryRetrieveInformationModel, scu);
  presID = findUncompressedPC(UID_MOVEStudyRootQueryRetrieveInformationModel, scu); 
  CHECK_FALSE(presID == 0);

  OFListIterator(QRResponse*) study = findResponses.begin(); 
  Uint32 studyCount = 1; 

  // Every while loop run will get all image for a specific study 
  while (study != findResponses.end() && result.good())
  { 
      // be sure we are not in the last response which does not have a dataset 
      if ( (*study)->m_dataset != NULL) 
      { 
      OFString studyInstanceUID; 
      result = (*study)->m_dataset->findAndGetOFStringArray(DCM_StudyInstanceUID, studyInstanceUID); 
      // only try to get study if we actually have study instance uid, otherwise skip it 
      CHECK(result.good());

      if (result.good()) 
      { 
          req->putAndInsertOFStringArray(DCM_StudyInstanceUID, studyInstanceUID); 
          // fetches all images of this particular study 
          result = scu.sendMOVERequest(presID, MOVEAPPLICATIONTITLE, req, NULL /* we are not interested into responses*/); 
          CHECK(result.good());
          if (result.good()) 
          { 
          DCMNET_INFO("Received study #" << std::setw(7) << studyCount << ": " << studyInstanceUID); 
          studyCount++; 
          } 
      }
      } 
      study++;
  } 

  while (!findResponses.empty())
  {
      delete findResponses.front();
      findResponses.pop_front();
  }
  /* Release association */ 
  
  scu.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
    
}


//#endif