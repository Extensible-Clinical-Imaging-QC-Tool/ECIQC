#include <limits>
#include <type_traits>
#include <iostream>
#include "catch.hpp"


#include <dcmtk/config/osconfig.h>

#ifdef WITH_THREADS

#include <dcmtk/dcmpstat/dcmpstat.h>
#include <dcmtk/dcmnet/scu.h>
#include <dcmtk/dcmnet/dstorscu.h>



#include "../src/Exception.hpp"
#include "../src/MyLibrary.hpp"

#include "../src/draft_receiver.hpp"

using namespace cpp_template;

 // Create a test SCU to send C-ECHO request
  struct TestSCU : DcmSCU, OFThread {
      OFCondition result;
    protected:
      void run(){
        negotiateAssociation();
        result = sendECHORequest(0);
        releaseAssociation();
        }
    };

  // Create a storage SCU that can send C-STORE requests.
  struct TestStorageSCU : DcmStorageSCU, OFThread {
      OFCondition result;
      OFCondition result2;
    protected:
      void run(){
        negotiateAssociation();
        Uint16 rspStatusCode = 0;

        // Images to be sent
        result = sendSTORERequest(0, "../DICOM_Images/1-1copy.dcm", 0, rspStatusCode);
        result2 = sendSTORERequest(0, "../DICOM_Images/1-01.dcm", 0, rspStatusCode);
        releaseAssociation();
        }
    };

// This tests handling of C-ECHO Association
TEST_CASE("Test C-ECHO Association"){
  
  Receiver pool(11112, "TestSCP");

  // Define presentation contexts for SCU
  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

  // Start listening
  pool.start();

  // Configure SCU and initialize
  OFVector<TestSCU*> scus(2);
  for (OFVector<TestSCU*>::iterator it1 = scus.begin(); it1 != scus.end(); ++it1)
      {
          *it1 = new TestSCU;
          (*it1)->setAETitle("PoolTestSCU");
          (*it1)->setPeerAETitle("TestSCP");
          (*it1)->setPeerHostName("localhost");
          (*it1)->setPeerPort(11112);
          (*it1)->addPresentationContext(UID_VerificationSOPClass, xfers);
          (*it1)->initNetwork();
      }
  OFStandard::sleep(5);
  

  // Start SCUs
  for (OFVector<TestSCU*>::const_iterator it2 = scus.begin(); it2 != scus.end(); ++it2)
        (*it2)->start();


  // Check the association.
  for (OFVector<TestSCU*>::iterator it3 = scus.begin(); it3 != scus.end(); ++it3)
      {
        (*it3)->join();
        CHECK((*it3)->result.good());
        delete *it3;
      };
  
  // Request shutdown.
  pool.request_stop();
  pool.join();

}

// This tests handling of C-STORE Association
TEST_CASE("Test C-STORE Association"){

  Receiver pool(11112, "TestSCP");

  // Define presentation contexts
  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

  // Define a separate transfer syntax needed for the X-ray image
  OFList<OFString> ts;
  ts.push_back(UID_LittleEndianImplicitTransferSyntax);
  
  pool.start();

  // Configure SCUs.
  OFVector<TestStorageSCU*> scus(2);
  for (OFVector<TestStorageSCU*>::iterator it1 = scus.begin(); it1 != scus.end(); ++it1)
      {
          *it1 = new TestStorageSCU;
          (*it1)->setAETitle("StoreTestSCU");
          (*it1)->setPeerAETitle("TestSCP");
          (*it1)->setPeerHostName("localhost");
          (*it1)->setPeerPort(11112);
          (*it1)->setVerbosePCMode(OFTrue);
          (*it1)->addPresentationContext(UID_VerificationSOPClass, xfers);
          (*it1)->addPresentationContext(UID_CTImageStorage, xfers);
          (*it1)->addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts);
          (*it1)->initNetwork();
      }
  
  OFStandard::sleep(5);

  // Start SCUs
  for (OFVector<TestStorageSCU*>::const_iterator it2 = scus.begin(); it2 != scus.end(); ++it2)
        (*it2)->start();

  // Check if C-STORE was successful
  for (OFVector<TestStorageSCU*>::iterator it3 = scus.begin(); it3 != scus.end(); ++it3)
      {
        (*it3)->join();
        CHECK((*it3)->result.good());
        CHECK((*it3)->result2.good());
        delete *it3;
      };
  
  // Request shutdown.
  pool.request_stop();
  pool.join();

}

// This tests basic configuration of the Receiver.
TEST_CASE("Test Receiver Configuration"){

    Receiver scppool(104, "ATestName");
    
    //scppool.setaetitle("ATestName");
    //scppool.setportnumber(104);

    CHECK(scppool.getConfig().getAETitle() == "ATestName");
    CHECK(scppool.getConfig().getPort() == 104);

    OFList<OFString> hostiplist;
    hostiplist.push_back("An IP address");

    OFList<OFString> peeraelist;
    peeraelist.push_back("A test name");
    
    scppool.setacceptableIPs(hostiplist);
    scppool.setcallingAETitles(peeraelist);

    CHECK(scppool.getacceptableIPs().size() == 1);
    CHECK(scppool.getcallingAETitles().size() == 1);
}

// This tests datasource check at hostname/IP level
TEST_CASE("Test hostname/IP check - Accept"){

  // Check that a specified hostname is accepted.
  Receiver pool(11112, "TestSCP");

  OFList<OFString> hostiplist;
  hostiplist.push_back("localhost");
    
  pool.setacceptableIPs(hostiplist);

  // Define presentation contexts for SCU
  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

  // Start listening
  pool.start();

  TestSCU scu;
  scu.setAETitle("PoolTestSCU");
  scu.setPeerAETitle("TestSCP");
  scu.setPeerHostName("localhost");
  scu.setPeerPort(11112);
  scu.addPresentationContext(UID_VerificationSOPClass, xfers);
  scu.initNetwork();

  OFStandard::sleep(5);

  scu.start();
  scu.join();
  CHECK(scu.result.good());
  
  // Request shutdown.
  pool.request_stop();
  pool.join();
}

TEST_CASE("Test hostname/IP check - Reject"){

  // Check that a non-specified hostname is rejected.
  Receiver poolrej(11112, "TestSCP");

  OFList<OFString> hostiplistrej;
  hostiplistrej.push_back("AnIPAddress");

  poolrej.setacceptableIPs(hostiplistrej);

  // Define presentation contexts for SCU
  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

  poolrej.start();

  TestSCU rejscu;
  rejscu.setAETitle("PoolTestSCU");
  rejscu.setPeerAETitle("TestSCP");
  rejscu.setPeerHostName("localhost");
  rejscu.setPeerPort(11112);
  rejscu.addPresentationContext(UID_VerificationSOPClass, xfers);
  rejscu.initNetwork();
  OFStandard::sleep(5);

  rejscu.start();
  rejscu.join();
  CHECK(rejscu.result.bad());
  // Request shutdown.
  poolrej.request_stop();
  poolrej.join();
}

// This tests datasource check at hostname/IP level
TEST_CASE("Test called AE Title check"){

  // Check that a specified AE title is accepted.
  Receiver pool(11112, "TestSCP");

  OFList<OFString> aetitles;
  aetitles.push_back("PoolTestSCU");
    
  pool.setcallingAETitles(aetitles);

  // Define presentation contexts for SCU
  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

  // Start listening
  pool.start();

  // Configure SCU and initialize
  TestSCU scu;
  scu.setAETitle("PoolTestSCU");
  scu.setPeerAETitle("TestSCP");
  scu.setPeerHostName("localhost");
  scu.setPeerPort(11112);
  scu.addPresentationContext(UID_VerificationSOPClass, xfers);
  scu.initNetwork();

  OFStandard::sleep(5);

  scu.start();
  scu.join();
  CHECK(scu.result.good());
  
  // Request shutdown.
  pool.request_stop();
  pool.join();

  // Check that a non-specified hostname is rejected.
  Receiver poolrej(11112, "TestSCP");

  OFList<OFString> aelistrej;
  aelistrej.push_back("A Title");

  poolrej.setcallingAETitles(aelistrej);

  poolrej.start();

  scu.initNetwork();
  OFStandard::sleep(5);

  scu.start();
  scu.join();
  CHECK(scu.result.bad());
  // Request shutdown.
  poolrej.request_stop();
  poolrej.join();
}

#endif