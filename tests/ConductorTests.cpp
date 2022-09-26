#include <limits>
#include <type_traits>
#include <iostream>
#include "catch.hpp"
#include <exception>


#include <dcmtk/config/osconfig.h>
#include "../src/conductor.hpp"


#ifdef WITH_THREADS
TEST_CASE("Test Conductor in sending C-STORE Request for a list of Passed Dataset","[CT]"){

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
    result2 = sendSTORERequest(0, "../DICOM_Images/1-1copy.dcm", 0, rspStatusCode);
    releaseAssociation();
        }
    };

    //Instantiate receiver class (Receiver class)
    Receiver testSCP(11112, "testSCP");

    //Instantiate conductor class 
    Conductor conductor("ConductorSCU","testSCP",11112,"localhost","ConductorSCP",104);
    conductor.run();

    // Define presentation contexts for SCU
    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

    // Define a separate transfer syntax needed for the X-ray image
    OFList<OFString> ts;
    ts.push_back(UID_LittleEndianImplicitTransferSyntax);

    // Execute C-STORE Request with TestSCU
    OFshared_ptr<OFList<DcmDataset>>  pDset(new OFList<DcmDataset>);
    testSCP.setpointer(pDset);

    //Receiver listen
    testSCP.start();

      // Configure SCUs.
    OFVector<TestStorageSCU*> scus2(1);
    for (OFVector<TestStorageSCU*>::iterator testSCU = scus2.begin(); testSCU != scus2.end(); ++testSCU)
        {
            *testSCU = new TestStorageSCU;
            (*testSCU)->setAETitle("StoreTestSCU");
            (*testSCU)->setPeerAETitle("ConductorSCP");
            (*testSCU)->setPeerHostName("localhost");
            (*testSCU)->setPeerPort(104);
            (*testSCU)->setVerbosePCMode(OFTrue);
            (*testSCU)->addPresentationContext(UID_VerificationSOPClass, xfers);
            (*testSCU)->addPresentationContext(UID_CTImageStorage, xfers);
            (*testSCU)->addPresentationContext(UID_MRImageStorage, xfers);
            (*testSCU)->addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts);
            (*testSCU)->initNetwork();
        }
    
    OFStandard::sleep(5);

    // Start test SCUs
    for (OFVector<TestStorageSCU*>::const_iterator it2 = scus2.begin(); it2 != scus2.end(); ++it2)
            (*it2)->start();

    // Check if C-STORE was successful
    for (OFVector<TestStorageSCU*>::iterator it3 = scus2.begin(); it3 != scus2.end(); ++it3)
        {
            (*it3)->join();
            if((*it3)->result1.bad())
                throw "C-STORE requests failed!";
            if((*it3)->result2.bad())
                throw "C-STORE requests failed!";
            delete *it3;
        };

    pDset->size();

    testSCP.request_stop();
    testSCP.join();

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