#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */ 
#include "dcmtk/dcmnet/diutil.h" 
#include "catch.hpp"

#include "../src/Exception.hpp"
#include "../src/MyLibrary.hpp"

#include "../src/test_scu.hpp"

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

TEST_CASE("Test C-ECHO Association with SCU","[ST]"){
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