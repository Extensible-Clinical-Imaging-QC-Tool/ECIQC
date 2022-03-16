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
                     std::string ReceiverAETitle, Uint16 ReceiverPortNumber, std::string ReceiverPortName)
                     :scu(SenderAETitle, ReceiverPortName, ReceiverPortNumber,ReceiverAETitle),
                      scp(ReceiverPortNumber, ReceiverAETitle){

                      }
 
// set optional variables
void Conductor::setOptional(/*all optional variables */) {
  /* setStorage() */
}

void Conductor::run() {
    
    // Receiver starts listening
    OFshared_ptr<OFList<DcmDataset>>  pDset(new OFList<DcmDataset>);
    scp.setpointer(pDset);
    scp.start();

    //Execute C-ECHO Request with SCU
     
    //Initialize network
    OFCondition result1 = scu.initNetwork();
    //if (result1.bad())
    //  throw "Network initialization failed!";

    
    //Negotiate association
    OFCondition result2 = scu.negotiateAssociation();
   // if (result2.bad())
     //   throw "Association negotiation failed!";


    //Check whether server is listening
    OFCondition result3 = scu.sendECHORequest(0);
   // if (result3.bad())
     //   throw "Send ECHO Request failed!";

    //Release association
    OFCondition result4 = scu.releaseAssociation();
   // if (result4.bad())
     //   throw "Association Released failed!";

    //Execute C-STORE Request with SCU
    //C-STORE Request for US MultiFrame Images, JPEG Baseline Process 1

    //Initialize network 
    
    OFCondition result = scu.initNetwork(); 
    if (result.bad())
        throw "Network initialization failed!";
    
    DcmFileFormat dfile;
    result = dfile.loadFile("../DICOM_Images/testtext.dcm");
    if (result.bad())
        throw "Failed to load Dicom File!";
    DcmDataset *data = dfile.getDataset();
    if (data == NULL)
        throw "Empty dataset!";


    // Negotiate Association  
    
    result = scu.negotiateAssociation(); 
    if (result.bad())
        throw "Association negotiation failed!";

    //Assemble and send C-STORE request. Check if C-STORE was successful.
    Uint16 rspStatusCode = 0;
    result = scu.sendSTORERequest(0, 0,data, rspStatusCode );
    if (result.bad()){   
        OFCondition status = data->saveFile("../DICOM_Images/archive_1.dcm");
        if (status.bad())
            throw "Failed to save file after failure of C-STORE Request!";
        }

    //Second attempt for successful C-STORE association

    //Extracting data from dicom file (CT Image Storage GE Systems- Explicit Little Endian)
    DcmFileFormat dfile2;
    result = dfile2.loadFile("../DICOM_Images/1-01.dcm");
    if (result.bad())
        throw "Failed to load Dicom File!";
    DcmDataset *data2 = dfile2.getDataset();
    if (data2 == NULL)
        throw "Empty dataset!";
    

    //Assemble and send C-STORE request. Check if C-STORE was successful.
    rspStatusCode = 0;
    result = scu.sendSTORERequest(0,  0,data2, rspStatusCode );
    if (result.bad()){   
        OFCondition status = data2->saveFile("../DICOM_Images/archive_2.dcm");
        if (status.bad())
            throw "Failed to save file after failure of C-STORE Request!";
        }
        

    //Release association. 
    result = scu.releaseAssociation();
    if (result.bad())
        throw "Association release failed!";


    //Request shutdown and stop listening. 
    scp.request_stop();
    scp.join();
    


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

    Conductor::~Conductor(){}
    

