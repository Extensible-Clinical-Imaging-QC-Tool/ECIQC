// #include


#include <dcmtk/config/osconfig.h>
#include <iostream>
#include <exception>
#include "conductor.hpp"




/* In between command line and constructor, have something that
separates the compulsory and optional variables. Constructor only
takes compulsory variables. Optional variables are then fed to
specific setX() functions */

// Constructor ()
// 2 config files, 2*(port number and port name)

Conductor::Conductor( std::string SenderAETitle,  Uint16 SenderPortNumber, std::string SenderPortName,
                     std::string ReceiverAETitle, Uint16 ReceiverPortNumber, std::string ReceiverPortName){
      //Declare SCU and SCP parameters
       
      }
 
// set optional variables
void Conductor::setOptional(/*all optional variables */) {
  /* setStorage() */
}

// Receiver




// Parser

// Metadata Editor

// Validator

// Sender

void Conductor::run() {
    
    
    // Receiver starts listening
    OFshared_ptr<OFList<DcmDataset>>  pDset(new OFList<DcmDataset>);
    Receiver scp(ReceiverPortNumber, ReceiverAETitle);
    scp.setpointer(pDset);
    scp.start();

    //Execute C-ECHO Request with SCU
    //set SCU parameters
    Sender scu(SenderAETitle, ReceiverPortName, ReceiverPortNumber, ReceiverAETitle);
    scu.setAETitle(SenderAETitle.c_str()); 
    scu.setPeerHostName(ReceiverPortName.c_str()); 
    scu.setPeerPort(ReceiverPortNumber); 
    scu.setPeerAETitle(ReceiverAETitle.c_str());

    //Define presentation contexts, propose all uncompressed TS
    OFList<OFString> ts;
    ts.push_back(UID_LittleEndianExplicitTransferSyntax); 
    ts.push_back(UID_BigEndianExplicitTransferSyntax); 
    ts.push_back(UID_LittleEndianImplicitTransferSyntax); 
    scu.addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, ts); 
    scu.addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, ts); 
    scu.addPresentationContext(UID_VerificationSOPClass, ts); 

    //Initialize network
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
    } 

