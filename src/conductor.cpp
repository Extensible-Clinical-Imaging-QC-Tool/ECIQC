#include <dcmtk/config/osconfig.h>
#include <iostream>
#include <exception>
#include "conductor.hpp"
#include <signal.h>

#define PRV_PrivateQuar DcmTag(0x1333, 0x0052, EVR_IS)


/* In between command line and constructor, have something that
separates the compulsory and optional variables. Constructor only
takes compulsory variables. Optional variables are then fed to
specific setX() functions */

// Constructor ()
// 2 config files, 2*(port number and port name)

Conductor::Conductor( std::string SenderAETitle, std::string PeerAETitle,  Uint16 PeerPortNumber, std::string PeerPortName,
                     std::string ReceiverAETitle, Uint16 ReceiverPortNumber)
                     :scu(SenderAETitle, PeerPortName, PeerPortNumber,PeerAETitle),
                      scp(ReceiverPortNumber, ReceiverAETitle), 
                      m_keep_running(1)  ,
                      m_received_pDset()
                      {

                      }
 
// set optional variables
void Conductor::setOptional(/*all optional variables */) {
  /* setStorage() */
}
void Conductor::int_handler(int dummy) {
    m_keep_running = 0;
}

void Conductor::pipeline(DcmDataset &dataset) {
    // calls parser
    // calls meta editror...
    //
    // sends dataset to either quarantine or receiver
}
void Conductor::run() {
    // Execute C-STORE Request with TestSCU
    OFshared_ptr<OFList<DcmDataset>>  received_pDset(new OFList<DcmDataset>);
    m_received_pDset = received_pDset;
    scp.setpointer(m_received_pDset);
    signal(SIGINT, int_handler);
    //Receiver listen
    scp.start();
    m_keep_running = 1;
    while (m_keep_running) {
        while (m_received_pDset->size() > 0) {
            DcmDataset& dataset = m_received_pDset->front(); 
            pipeline(dataset);
            // aquire lock
            m_received_pDset->pop_front()
            // release lock
        }
        OFStandard::sleep(1);
    }
    

    
    //OFStandard::sleep(5);

    std::cout<<"Conductor receiver (initialise) pDset size ="<<m_received_pDset->size()<<"\n";



// //TODO: Include Parser class
//     OFString config = "../schema/useCase.json";
//     Parser parser{config};

    //  OFshared_ptr<OFList<DcmDataset>> passed_pDset;
    //  OFshared_ptr<OFList<DcmDataset>> quarantine_pDset;
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

//C-ECHO Request(Sender Class)
 

        std::cout<<"Conductor receiver (run) pDset size ="<<m_received_pDset->size()<<"\n";
//     while (! received_pDset ->empty())
//     {
//     OFCondition result1 = scu.initNetwork();
//     if (result1.bad())
//       throw "Network initialization failed!";
    
 
//     //Negotiate association
//     OFCondition result2 = scu.negotiateAssociation();
//     if (result2.bad())
//       throw "Association negotiation failed!";

//    std::cout << "Help!"<< "\n";

//     //Check whether server is listening
//     OFCondition result3 = scu.sendECHORequest(0);
//     if (result3.bad())
//         throw "Send ECHO Request failed!";

//     //Release association
//     OFCondition result4 = scu.releaseAssociation();
//     if (result4.bad())
//         throw "Association Released failed!";

// //Execute first C-STORE Request with SCU
//     //C-STORE Request for passed_pDset

//     //Initialize network 
    
//     OFCondition result5 = scu.initNetwork(); 
//     if (result5.bad())
//         throw "Network initialization failed!";
    
    
//    // Remove first item 
//     DcmDataset* passed_dset = &(received_pDset->front());


//     // Negotiate Association  
    
//     OFCondition result7 = scu.negotiateAssociation(); 
//     if (result7.bad())
//         throw "Association negotiation failed!";

//     //Assemble and send first C-STORE request. Check if C-STORE was successful.
//     Uint16 rspStatusCode = 0;
//     OFCondition result8 = scu.sendSTORERequest(0, 0,passed_dset, rspStatusCode );
//     if (result8.bad()){   
//         OFCondition status = passed_dset->saveFile("../DICOM_Images/archive_1.dcm");
//         if (status.bad())
//             throw "Failed to save passed file after failure of C-STORE Request!";
//         }

//     std::cout<<"Conductor Receiver pDset size = "<<received_pDset->size()<<"\n";
//     //Release association. 
//     OFCondition result11 = scu.releaseAssociation();
//     if (result11.bad())
//         throw "Association release failed!";

//     received_pDset->pop_front();

//     } 

    
// //Execute second C-STORE Request with SCU
//     //C-STORE Request for quarantine_pDset

//     //Initialize network 
    
//     OFCondition result12 = scu.initNetwork(); 
//     if (result12.bad())
//         throw "Network initialization failed!";
    
//     while (! quarantine_pDset ->empty())
//     {
//    // Remove first item 
//     DcmDataset* quarantine_dset = &(quarantine_pDset->front());


//     // Negotiate Association  
    
//     OFCondition result13 = scu.negotiateAssociation(); 
//     if (result13.bad())
//         throw "Association negotiation failed!";

//     //Assemble and send first C-STORE request. Check if C-STORE was successful.
//     Uint16 rspStatusCode = 0;
//     OFCondition result14 = scu.sendSTORERequest(0, 0,quarantine_dset, rspStatusCode );
//     if (result14.bad()){   
//         OFCondition status = quarantine_dset->saveFile("../DICOM_Images/archive_2.dcm");
//         if (status.bad())
//             throw "Failed to save quarantine file after failure of C-STORE Request!";
//         }


//     //Release association. 
//     OFCondition result15 = scu.releaseAssociation();
//     if (result15.bad())
//         throw "Association release failed!";

//     quarantine_pDset->pop_front();

//     } 

// ~pDset;
    

    
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



    std::cout<<"Conductor pDset size ="<<m_received_pDset->size()<<"\n";
    scp.request_stop();
    scp.join();
}
// ---------------------------------------------------------------------------------------------------------
    Conductor::~Conductor(){}
// ----------------------------------------------------------------------------------------------------------

