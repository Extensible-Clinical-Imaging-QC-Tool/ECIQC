#include <limits>
#include <type_traits>
#include <iostream>
#include "catch.hpp"
#include <exception>

#include <dcmtk/config/osconfig.h>
#include "../src/conductor.hpp"


#ifdef WITH_THREADS
TEST_CASE("Test Conductor in sending C-STORE Request for a list of Passed Datasets","[CT]"){

    // Create a storage SCU that can send C-STORE requests.
    struct TestStorageSCU : DcmStorageSCU, OFThread {
      OFCondition result1, result2;
      OFCondition status;
    protected:
    void run(){
    Uint16 rspStatusCode = 0;
    negotiateAssociation();

    // Images to be sent
    result1 = sendSTORERequest(0, "../DICOM_Images/1-1copy.dcm", 0, rspStatusCode);
    result2 = sendSTORERequest(0, "../DICOM_Images/test2.dcm", 0, rspStatusCode);
    releaseAssociation();
        }
    };


    // struct TestConductor : Conductor, OFThread {
    //     TestConductor( );
    //     protected:
    //     void run(){
    //         run();
    //     }
    // };
    // //Instantiate receiver class (Receiver class)
    // Receiver testSCP(5035, "testSCP");

    // // Execute C-STORE Request with TestSCU
    // OFshared_ptr<OFList<DcmDataset>>  pDset(new OFList<DcmDataset>);
    // testSCP.setpointer(pDset);

    // //Receiver listen
    // testSCP.start();

    
    // Define presentation contexts for SCU
    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

    // Define a separate transfer syntax needed for the X-ray image
    OFList<OFString> ts;
    ts.push_back(UID_LittleEndianImplicitTransferSyntax);

    

      // Configure SCUs.
    OFVector<TestStorageSCU*> scus2(1);
    for (OFVector<TestStorageSCU*>::iterator testSCU = scus2.begin(); testSCU != scus2.end(); ++testSCU)
        {
            *testSCU = new TestStorageSCU;
            (*testSCU)->setAETitle("StoreTestSCU");
            (*testSCU)->setPeerAETitle("ConductorSCP");
            (*testSCU)->setPeerHostName("localhost");
            (*testSCU)->setPeerPort(11112);
            (*testSCU)->setVerbosePCMode(OFTrue);
            (*testSCU)->addPresentationContext(UID_VerificationSOPClass, xfers);
            (*testSCU)->addPresentationContext(UID_CTImageStorage, xfers);
            (*testSCU)->addPresentationContext(UID_MRImageStorage, xfers);
            (*testSCU)->addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts);
            (*testSCU)->initNetwork();
        }
    
    //Instantiate conductor class 
    Conductor testconductor("ConductorSCU","testSCP",5035,"localhost","ConductorSCP",11112);
    testconductor.run();
    

    // Start test SCUs
    for (OFVector<TestStorageSCU*>::const_iterator it2 = scus2.begin(); it2 != scus2.end(); ++it2)
            (*it2)->start();

    
    // Check if C-STORE was successful
    for (OFVector<TestStorageSCU*>::iterator it3 = scus2.begin(); it3 != scus2.end(); ++it3)
        {
            (*it3)->join();
            if((*it3)->result1.bad())
                throw "First C-STORE request failed!";
            if((*it3)->result2.bad())
                throw "Second C-STORE request failed!";
            delete *it3;
        };
    // pDset->size();
    
    
    // testSCP.request_stop();
    // testSCP.join();

 

    // delete &(*pDset);
    

}
// TEST_CASE("Test Conductor config", "[CT]")
// {
//     Conductor conductor("TestSCU", 104, "localhost","TestSCP", 11112, "localhost");
//     //conductor.scp;
//     conductor.run();
//     CHECK(conductor.scp.getConfig().getAETitle() == "TestSCP");
//     CHECK(conductor.scu.getAETitle() == "TestSCU");
// }


#endif
