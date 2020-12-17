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

// This tests handling of C-ECHO Association
TEST_CASE("Test C-ECHO Association"){

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
  
  Receiver pool;

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

  // Create test SCU that can send C-STORE requests.
  struct TestSCU : DcmStorageSCU, OFThread {
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

  Receiver pool;

  // Define presentation contexts
  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

  // Define a separate transfer syntax needed for the X-ray image
  OFList<OFString> ts;
  ts.push_back(UID_LittleEndianImplicitTransferSyntax);
  
  pool.start();

  // Configure SCUs.
  OFVector<TestSCU*> scus(2);
  for (OFVector<TestSCU*>::iterator it1 = scus.begin(); it1 != scus.end(); ++it1)
      {
          *it1 = new TestSCU;
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
  

  // Start SCUs
  for (OFVector<TestSCU*>::const_iterator it2 = scus.begin(); it2 != scus.end(); ++it2)
        (*it2)->start();

  // Check if C-STORE was successful
  for (OFVector<TestSCU*>::iterator it3 = scus.begin(); it3 != scus.end(); ++it3)
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

TEST_CASE("Test Receiver Configuration"){

    Receiver scppool;
    
    scppool.set_name("ATestName");

    CHECK(scppool.getConfig().getAETitle() == "ATestName");

    OFList<OFString> hostiplist;
    hostiplist.push_back("An IP address");
    
    scppool.setacceptableIPs(hostiplist);

    CHECK(scppool.getacceptableIPs().size() == 1);
}

TEST_CASE("Test IP check"){

  std::cout << "Testing IP Check";

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
  
  Receiver pool;

  OFList<OFString> hostiplist;
  hostiplist.push_back("An IP address");
    
  pool.setacceptableIPs(hostiplist);
  std::cout<<pool.getacceptableIPs().size()<<'\n';

  // Define presentation contexts for SCU
  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

  // Start listening
  pool.start();

  // Configure SCU and initialize
  OFVector<TestSCU*> scus(1);
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

  

  // Start SCUs
  for (OFVector<TestSCU*>::const_iterator it2 = scus.begin(); it2 != scus.end(); ++it2)
        (*it2)->start();


  // Check the association.
  for (OFVector<TestSCU*>::iterator it3 = scus.begin(); it3 != scus.end(); ++it3)
      {
        (*it3)->join();
        CHECK((*it3)->result.bad());
        delete *it3;
      };
  
  // Request shutdown.
  pool.request_stop();
  pool.join();
}

#endif