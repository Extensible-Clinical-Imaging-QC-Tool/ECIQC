// #include


#include <dcmtk/config/osconfig.h>
#include <iostream>
#include <exception>
#include "conductor.hpp"

#define PRV_PrivateQuar DcmTag(0x1333, 0x0052, EVR_IS)


/* In between command line and constructor, have something that
separates the compulsory and optional variables. Constructor only
takes compulsory variables. Optional variables are then fed to
specific setX() functions */

// Constructor ()
// 2 config files, 2*(port number and port name)

Conductor::Conductor( std::string SenderAETitle,  Uint16 SenderPortNumber, std::string SenderPortName,
                     std::string ReceiverAETitle, Uint16 ReceiverPortNumber, std::string ReceiverPortName)
                     :scu(SenderAETitle, ReceiverPortName, ReceiverPortNumber,ReceiverAETitle),
                      scp(ReceiverPortNumber, ReceiverAETitle),
                      testSCP(ReceiverPortNumber, ReceiverAETitle){

                      }
 
// set optional variables
void Conductor::setOptional(/*all optional variables */) {
  /* setStorage() */
}

void Conductor::run() {
    
    //Create a test SCU to send C-ECHO request
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
      OFCondition status;
    protected:
    void run(){
    Uint16 rspStatusCode = 0;
    negotiateAssociation();

    // Images to be sent
    result = sendSTORERequest(0, "../DICOM_Images/1-1copy.dcm", 0, rspStatusCode);
    releaseAssociation();
        }
    };

    //Instantiate receiver class (Receiver class)
    

    // Define presentation contexts for SCU
    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

    // Define a separate transfer syntax needed for the X-ray image
    OFList<OFString> ts;
    ts.push_back(UID_LittleEndianImplicitTransferSyntax);
    // Receiver starts listenings
    
    scp.start();

    // Configure SCU and initialize
    OFVector<TestSCU*> scus(1);
    for (OFVector<TestSCU*>::iterator testSCU = scus.begin(); testSCU != scus.end(); ++testSCU)
        {
            *testSCU = new TestSCU;
            (*testSCU)->setAETitle("TestSCU");
            (*testSCU)->setPeerAETitle("SCP");
            (*testSCU)->setPeerHostName("localhost");
            (*testSCU)->setPeerPort(11112);
            (*testSCU)->addPresentationContext(UID_VerificationSOPClass, xfers);
            (*testSCU)->initNetwork();
        }
    OFStandard::sleep(5);

    // Start SCUs
    for (OFVector<TestSCU*>::const_iterator it2 = scus.begin(); it2 != scus.end(); ++it2)
            (*it2)->start();

    // Check the association.
    for (OFVector<TestSCU*>::iterator it3 = scus.begin(); it3 != scus.end(); ++it3)
        {
            (*it3)->join();
            if ((*it3)->result.bad())
                throw "C-ECHO Request failed!";
            delete *it3;
        };
  
    // Request shutdown.
    scp.request_stop();
    scp.join();

    // Execute C-STORE Request with TestSCU
    OFshared_ptr<OFList<DcmDataset>>  pDset(new OFList<DcmDataset>);
    scp.setpointer(pDset);

    //Receiver listen
    scp.start();

      // Configure SCUs.
    OFVector<TestStorageSCU*> scus2(1);
    for (OFVector<TestStorageSCU*>::iterator testSCU = scus2.begin(); testSCU != scus2.end(); ++testSCU)
        {
            *testSCU = new TestStorageSCU;
            (*testSCU)->setAETitle("StoreTestSCU");
            (*testSCU)->setPeerAETitle("SCP");
            (*testSCU)->setPeerHostName("localhost");
            (*testSCU)->setPeerPort(11112);
            (*testSCU)->setVerbosePCMode(OFTrue);
            (*testSCU)->addPresentationContext(UID_VerificationSOPClass, xfers);
            (*testSCU)->addPresentationContext(UID_CTImageStorage, xfers);
            (*testSCU)->addPresentationContext(UID_MRImageStorage, xfers);
            (*testSCU)->addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts);
            (*testSCU)->initNetwork();
        }
    
    OFStandard::sleep(5);

    // Start SCUs
    for (OFVector<TestStorageSCU*>::const_iterator it2 = scus2.begin(); it2 != scus2.end(); ++it2)
            (*it2)->start();

    // Check if C-STORE was successful
    for (OFVector<TestStorageSCU*>::iterator it3 = scus2.begin(); it3 != scus2.end(); ++it3)
        {
            (*it3)->join();
            if((*it3)->result.bad())
                throw "C-STORE requests failed!";
            delete *it3;
        };

    scp.request_stop();
    scp.join();
// //TODO: Include Parser class
//     OFString config = "../schema/useCase.json";
//     Parser parser{config};

//     OFshared_ptr<OFList<DcmDataset>> passed_pDset;
//     OFshared_ptr<OFList<DcmDataset>> quarantine_pDset;
//     DcmTagKey quarTagKey = PRV_PrivateQuar;
//     OFString quar_bool_str;

//     // Loop through DcmDatasets:
//     while (! pDset->empty())
//         {
//             // Remove first item 
//             DcmDataset* dset = &(pDset->front());
//             parser.setDicomDset(dset);
            
//             DcmDataset* edited_dset = parser.parse();

//             OFString quarantine_tag = "(0000,0000)";

//             dset->findAndGetOFString(quarTagKey, quar_bool_str);
//             if (quar_bool_str == 1 || quar_bool_str == "1"){
//                 // edited_dset into passed_pDset
//             }
//             else {
//                 // edited_dset into quarantine_pDset
//             }
//             // decide whether to store in passed/quarantine
//             // check for a "quarantine" flag
//             pDset->pop_front();
//         };.

    //Execute C-ECHO Request with test SCP (Sender Class)
    //Create a test SCP to receive C-ECHO and C-STORE requests.
    //Receiver testSCP(ReceiverPortNumber,ReceiverAETitle);
    //OFshared_ptr<OFList<DcmDataset>>  pt(new OFList<DcmDataset>);
    testSCP.setpointer(pDset);
    testSCP.start();
    /*
    struct TestSCP : DQDSCPPool<ReceiverThread, DQDBaseSCPPool, DQDBaseSCPPool::DQDBaseSCPWorker>, OFThread {
      Uint16 ReceiverPortNumber;
      std::string ReceiverAETitle;
      OFCondition result;
     protected:
        void run(){
            result = listen();
        }

    
    };
    
    OFVector<TestSCP*> scps(1);
    for (OFVector<TestSCP*>::iterator it1 = scps.begin(); it1 != scps.end(); ++it1){
        (*it1) = new TestSCP;
        OFString title = ReceiverAETitle.c_str();
        (*it1)->getConfig().setPort(ReceiverPortNumber) ;
        (*it1)->getConfig().setAETitle(title) ;
        (*it1)->setMaxThreads(1);
        (*it1)->getConfig().setConnectionBlockingMode(DUL_NOBLOCK);
        (*it1)->getConfig().setConnectionTimeout(5);
        (*it1)->getConfig().setVerbosePCMode(OFTrue);

        // Add presentation context to be handled
        OFList<OFString> ts2;
        ts2.push_back(UID_LittleEndianExplicitTransferSyntax);
        ts2.push_back(UID_LittleEndianImplicitTransferSyntax);
        ts2.push_back(UID_BigEndianExplicitTransferSyntax);
        ts2.push_back(UID_JPEGProcess14SV1TransferSyntax);
        ts2.push_back(UID_JPEGProcess1TransferSyntax);
        for (size_t n = 0; n < numberOfDcmLongSCUStorageSOPClassUIDs; n++)
        {
            (*it1)->getConfig().addPresentationContext(dcmLongSCUStorageSOPClassUIDs[n], ts2);
        }
        (*it1)->getConfig().addPresentationContext(UID_VerificationSOPClass, ts2);
        
        
        OFshared_ptr<OFList<DcmDataset>>  pt(new OFList<DcmDataset>);
        (*it1)->setpointer(pt);

        std::cout<<(*it1)->getConfig().getAETitle() << "\n";
        
    }

    // Start Test SCP
    for (OFVector<TestSCP*>::const_iterator it2 = scps.begin(); it2 != scps.end(); ++it2){
            (*it2)->start();
            std::cout<<(*it2)->getConfig().getAETitle() << "\n";
    }
    */
 
    //Initialize nsetwork
    OFCondition result1 = scu.initNetwork();
    if (result1.bad())
      throw "Network initialization failed!";
    
    //Negotiate association
    OFCondition result2 = scu.negotiateAssociation();
    if (result2.bad())
        throw "Association negotiation failed!";


    //Check whether server is listening
    OFCondition result3 = scu.sendECHORequest(0);
    if (result3.bad())
        throw "Send ECHO Request failed!";

    //Release association
    OFCondition result4 = scu.releaseAssociation();
    if (result4.bad())
        throw "Association Released failed!";

    //Execute first C-STORE Request with SCU
    //C-STORE Request for US MultiFrame Images, JPEG Baseline Process 1

    //Initialize network 
    
    OFCondition result5 = scu.initNetwork(); 
    if (result5.bad())
        throw "Network initialization failed!";
    
    DcmFileFormat dfile;
    OFCondition result6 = dfile.loadFile("../DICOM_Images/testtext.dcm");
    if (result6.bad())
        throw "Failed to load Dicom File!";
    DcmDataset *data = dfile.getDataset();
    if (data == NULL)
        throw "Empty dataset!";


    // Negotiate Association  
    
    OFCondition result7 = scu.negotiateAssociation(); 
    if (result7.bad())
        throw "Association negotiation failed!";

    //Assemble and send first C-STORE request. Check if C-STORE was successful.
    Uint16 rspStatusCode = 0;
    OFCondition result8 = scu.sendSTORERequest(0, 0,data, rspStatusCode );
    if (result8.bad()){   
        OFCondition status = data->saveFile("../DICOM_Images/archive_1.dcm");
        if (status.bad())
            throw "Failed to save file after failure of C-STORE Request!";
        }

    //Second C-STORE request

    //Extracting data from dicom file (CT Image Storage GE Systems- Explicit Little Endian)
    DcmFileFormat dfile2;
    OFCondition result9 = dfile2.loadFile("../DICOM_Images/1-01.dcm");
    if (result9.bad())
        throw "Failed to load Dicom File!";
    DcmDataset *data2 = dfile2.getDataset();
    if (data2 == NULL)
        throw "Empty dataset!";
    

    //Assemble and send C-STORE request. Check if C-STORE was successful.
    rspStatusCode = 0;
    OFCondition result10 = scu.sendSTORERequest(0,  0,data2, rspStatusCode );
    if (result10.bad()){   
        OFCondition status = data2->saveFile("../DICOM_Images/archive_2.dcm");
        if (status.bad())
            throw "Failed to save file after failure of C-STORE Request!";
        }
        

    //Release association. 
    OFCondition result11 = scu.releaseAssociation();
    if (result11.bad())
        throw "Association release failed!";


    testSCP.request_stop();
    testSCP.join();
            


    } 


    // Validation Parser

    /* std::string mode = 'val' ;
    p1.setDicomFile(pDset);
    OFBool pass = p1.pRun(mode);
    s(qPortNum, qPortName);
    // ME Parser
    if(pass) {
      mode = "me";
      p2.setDicomFile(pDset);
      p2.pRun(mode);

      s(sPortNum, sPortName);
    */
// ---------------------------------------------------------------------------------------------------------
    Conductor::~Conductor(){}
// ----------------------------------------------------------------------------------------------------------

