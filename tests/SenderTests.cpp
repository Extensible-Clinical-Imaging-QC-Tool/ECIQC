#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */ 
#include "dcmtk/dcmnet/diutil.h" 
#include "catch.hpp"

#include "../src/misc/Exception.hpp"
#include "../src/misc/MyLibrary.hpp"

#include "../src/communication/test_scu.hpp"

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





TEST_CASE("Test network initialization with SCU","[ST]"){
  /* Setup DICOM connection parameters */ 
  OFLog::configure(OFLogger::DEBUG_LOG_LEVEL); 
  DcmTestSCU scu; 
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

}

TEST_CASE("Test negotiation association with SCU","[ST]"){
  /* Setup DICOM connection parameters */ 
  OFLog::configure(OFLogger::DEBUG_LOG_LEVEL); 
  DcmTestSCU scu; 
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

}

TEST_CASE("Test C-ECHO Request with SCU","[ST]"){
  /* Setup DICOM connection parameters */ 
  OFLog::configure(OFLogger::DEBUG_LOG_LEVEL); 
  DcmTestSCU scu; 
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

}

TEST_CASE("Test C-FIND request with SCU","[ST]"){
  /* Setup DICOM connection parameters */ 
  OFLog::configure(OFLogger::DEBUG_LOG_LEVEL); 
  DcmTestSCU scu; 
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

  /* Assemble and send C-FIND request */ 
  OFList<QRResponse*> findResponses; 
  DcmDataset req; 
  req.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "STUDY"); 
  req.putAndInsertOFStringArray(DCM_StudyInstanceUID, ""); 
  T_ASC_PresentationContextID presID = findUncompressedPC(UID_FINDStudyRootQueryRetrieveInformationModel, scu);
  CHECK (presID != 0);
  
  result = scu.sendFINDRequest(presID, &req, &findResponses); 
  CHECK (result.good());
  DCMNET_INFO("There are " << findResponses.size() << " studies available");
  CHECK(findResponses.size() == 242) ;


}

TEST_CASE("Test C-MOVE request with SCU","[ST]"){
  /* Setup DICOM connection parameters */ 
  OFLog::configure(OFLogger::DEBUG_LOG_LEVEL); 
  DcmTestSCU scu; 
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

  /* Assemble and send C-FIND request */ 
  OFList<QRResponse*> findResponses; 
  DcmDataset req; 
  req.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "STUDY"); 
  req.putAndInsertOFStringArray(DCM_StudyInstanceUID, ""); 
  T_ASC_PresentationContextID presID = findUncompressedPC(UID_FINDStudyRootQueryRetrieveInformationModel, scu);
  CHECK (presID != 0);
  
  result = scu.sendFINDRequest(presID, &req, &findResponses); 
  CHECK (result.good());
  DCMNET_INFO("There are " << findResponses.size() << " studies available");
  CHECK(findResponses.size() == 242) ;

  /* Assemble and send C-MOVE request, for each study identified above*/ 
  presID = findUncompressedPC(UID_MOVEStudyRootQueryRetrieveInformationModel, scu); 
  CHECK (presID !=0);
  if (presID == 0) 
  { 
    DCMNET_ERROR("There is no uncompressed presentation context for Study Root MOVE"); 
    return ; 
  } 
  OFListIterator(QRResponse*) study = findResponses.begin() ;
  Uint32 studyCount = 1; 
  OFBool failed = OFFalse; 
  
  // Every while loop run will get all image for a specific study 
  while (study == findResponses.begin() && result.good())
  { 
    // be sure we are not in the last response which does not have a dataset 
    if ( (*study)->m_dataset != NULL) 
    { 
      OFString studyInstanceUID; 
      result = (*study)->m_dataset->findAndGetOFStringArray(DCM_StudyInstanceUID, studyInstanceUID); 
      // only try to get study if we actually have study instance uid, otherwise skip it 
      CHECK (result.good());
      if (result.good()) 
      { 
        req.putAndInsertOFStringArray(DCM_StudyInstanceUID, studyInstanceUID); 
        // fetches all images of this particular study 
        result = scu.sendMOVERequest(presID, MOVEAPPLICATIONTITLE, &req, NULL /* we are not interested into responses*/); 
        CHECK (result.good());
        if (result.good()) 
        { 
          DCMNET_INFO("Received study #" << std::setw(7) << studyCount << ": " << studyInstanceUID); 
          studyCount++; 
        } 
      }
    } 
    study++;
  } 
  if (result.bad()) 
  { 
    DCMNET_ERROR("Unable to retrieve all studies: " << result.text()); 
  }
  while (!findResponses.empty())
  {
    delete findResponses.front();
    findResponses.pop_front();
  }
}

TEST_CASE("Test Release Association with SCU","[ST]"){
  /* Setup DICOM connection parameters */ 
  OFLog::configure(OFLogger::DEBUG_LOG_LEVEL); 
  DcmTestSCU scu; 
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

  /* Assemble and send C-FIND request */ 
  OFList<QRResponse*> findResponses; 
  DcmDataset req; 
  req.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "STUDY"); 
  req.putAndInsertOFStringArray(DCM_StudyInstanceUID, ""); 
  T_ASC_PresentationContextID presID = findUncompressedPC(UID_FINDStudyRootQueryRetrieveInformationModel, scu);
  CHECK (presID != 0);
  
  result = scu.sendFINDRequest(presID, &req, &findResponses); 
  CHECK (result.good());
  DCMNET_INFO("There are " << findResponses.size() << " studies available");
  CHECK(findResponses.size() == 242) ;

  /* Assemble and send C-MOVE request, for each study identified above*/ 
  presID = findUncompressedPC(UID_MOVEStudyRootQueryRetrieveInformationModel, scu); 
  CHECK (presID !=0);
  if (presID == 0) 
  { 
    DCMNET_ERROR("There is no uncompressed presentation context for Study Root MOVE"); 
    return ; 
  } 
  OFListIterator(QRResponse*) study = findResponses.begin() ;
  Uint32 studyCount = 1; 
  OFBool failed = OFFalse; 
  
  // Every while loop run will get all image for a specific study 
  while (study == findResponses.begin() && result.good())
  { 
    // be sure we are not in the last response which does not have a dataset 
    if ( (*study)->m_dataset != NULL) 
    { 
      OFString studyInstanceUID; 
      result = (*study)->m_dataset->findAndGetOFStringArray(DCM_StudyInstanceUID, studyInstanceUID); 
      // only try to get study if we actually have study instance uid, otherwise skip it 
      CHECK (result.good());
      if (result.good()) 
      { 
        req.putAndInsertOFStringArray(DCM_StudyInstanceUID, studyInstanceUID); 
        // fetches all images of this particular study 
        result = scu.sendMOVERequest(presID, MOVEAPPLICATIONTITLE, &req, NULL /* we are not interested into responses*/); 
        CHECK (result.good());
        if (result.good()) 
        { 
          DCMNET_INFO("Received study #" << std::setw(7) << studyCount << ": " << studyInstanceUID); 
          studyCount++; 
        } 
      }
    } 
    study++;
  } 
  if (result.bad()) 
  { 
    DCMNET_ERROR("Unable to retrieve all studies: " << result.text()); 
  }
  while (!findResponses.empty())
  {
    delete findResponses.front();
    findResponses.pop_front();
  }
  /* Release association */ 
  scu.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
  
}







