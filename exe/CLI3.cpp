#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/oflog/fileap.h"
#include "../src/communication/sender.hpp"
#include "communication/receiver.hpp"
//#include "conductor.hpp"
#include <exception>

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
namespace po = boost::program_options;

int main(int argc, char** argv){

Uint16 ReceiverPortNumber;
OFString ReceiverPortName;
OFString ReceiverAETitle;
//Uint16 SenderPortNumber;
//OFString SenderPortName;
OFString SenderAETitle;

try {
    po::options_description ECIQC("Options");
    ECIQC.add_options()
        ("help", "produce help message")
        ("SenderAETitle", po::value<OFString>(&SenderAETitle)->default_value("TestSCU"), "set Sender AE Title")
        //("SenderPortNumber", po::value<Uint16>(&SenderPortNumber)->default_value(104), "set Sender Port Number")
        //("SenderPortName", po::value<OFString>(&SenderPortName)->default_value("localhost"),"set Sender Port Name")
        ("ReceiverAETitle", po::value<OFString>(&ReceiverAETitle)->default_value("TestSCP"), "set Receiver AE Title")
        ("ReceiverPortNumber", po::value<Uint16>(&ReceiverPortNumber)->default_value(11112), "set Receiver Port Number")
        ("ReceiverPortName", po::value<OFString>(&ReceiverPortName)->default_value("localhost"), "set Receiver Port Name");
        
        
    po::variables_map vm;
    //po::store(po::parse_command_line(argc, argv, ECIQC), vm); // use this if want user to enter configuration settings as arguments from command line directly
    po::store(po::parse_command_line(argc, argv, ECIQC),vm); 
    po::notify(vm);

    if (vm.count("help")) {
        cout << ECIQC << "\n";
        return 0;
      }

     if (vm.count("SenderAETitle")) {
            cout << "Sender Application Entity Title was set to " 
                 << vm["SenderAETitle"].as<OFString>() << ".\n";
        } else {
            cout << "Sender Application Entity Title was not set.\n";
        } 
    /*
    if (vm.count("SenderPortNumber")) {
            cout << "Sender Port Number was set to " 
                 << vm["SenderPortNumber"].as<Uint16>() << ".\n";
        } else {
            cout << "Sender Port Number was not set.\n";
        }
    
    if (vm.count("SenderPortName")) {
            cout << "Sender Port Name was set to " 
                 << vm["SenderPortName"].as<OFString>() << ".\n";
        } else {
            cout << "Sender Port Name was not set.\n";
        }
    */

    if (vm.count("ReceiverAETitle")) {
            cout << "Receiver Application Entity Title was set to " 
                 << vm["ReceiverAETitle"].as<OFString>() << ".\n";
        } else {
            cout << "Receiver Application Entity Title was not set.\n";
        } 

    if (vm.count("ReceiverPortNumber")) {
            cout << "Receiver Port Number was set to " 
                 << vm["ReceiverPortNumber"].as<Uint16>() << ".\n";
        } else {
            cout << "Receiver Port Number was not set.\n";
        }
    if (vm.count("ReceiverPortName")) {
            cout << "Receiver Port Name was set to " 
                 << vm["ReceiverPortName"].as<OFString>() << ".\n";
        } else {
            cout << "Receiver Port Name was not set.\n";
        }   

    //Specify log pattern in .log file.
    
    OFunique_ptr<dcmtk::log4cplus::Layout> layout(new dcmtk::log4cplus::PatternLayout("%D{%Y-%m-%d %H:%M:%S.%q} %5p: %m%n"));
    /* Denote that a log file should be used that is appended to. The file is re-created every
        time the code gets to this point.
    */
    dcmtk::log4cplus::SharedAppenderPtr logfile(new dcmtk::log4cplus::FileAppender("CLI3.log"));
    //logfile->setLayout(OFmove(layout));

    /* make sure that only the file logger is used */
    dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
    log.removeAllAppenders();
    log.addAppender(logfile);
    log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL);
    //Initiate Receiver
    OFshared_ptr<OFList<DcmDataset>>  pt(new OFList<DcmDataset>);
    Receiver pool(ReceiverPortNumber, ReceiverAETitle);
    pool.setpointer(pt);  

     //Start listening
    pool.start();

    //Execute C-ECHO Request with SCU
    Sender scu(SenderAETitle, ReceiverPortName, ReceiverPortNumber, ReceiverAETitle);
    // set AE titles 
    scu.setAETitle(SenderAETitle); 
    scu.setPeerHostName(ReceiverPortName); 
    scu.setPeerPort(ReceiverPortNumber); 
    scu.setPeerAETitle(ReceiverAETitle);

   

    // Define presentation contexts, propose all uncompressed transfer syntaxes 
    OFList<OFString> ts; 
    ts.push_back(UID_LittleEndianExplicitTransferSyntax); 
    ts.push_back(UID_BigEndianExplicitTransferSyntax); 
    ts.push_back(UID_LittleEndianImplicitTransferSyntax); 
    scu.addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, ts); 
    scu.addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, ts); 
    scu.addPresentationContext(UID_VerificationSOPClass, ts); 
    // Initialize network / 
    OFCondition result = scu.initNetwork(); 
    if (result.bad())
        throw "Network initialization failed!";
    //Negotiate association 
    result = scu.negotiateAssociation();
    if (result.bad())
        throw "Association negotiation failed!";

    //Check whether the server is listening//
    result = scu.sendECHORequest(0);
    if (result.bad())
        throw "Send ECHO Request failed!";
    //Release association 
    
    result = scu.releaseAssociation();
    if (result.bad())
        throw "Association Released failed!";
        
    
    

    //Execute C-STORE Request with SCU
    //C-STORE Request for US MultiFrame Images, JPEG Baseline Process 1 - Error: not enough disk space on the disk
    
    // Define a separate transfer syntax needed for the X-ray image
    OFList<OFString> xfer;
    xfer.push_back(UID_LittleEndianImplicitTransferSyntax);
    //Define a separate transfer syntax needed for multiframe US image
    OFList<OFString> xfer2;
    xfer2.push_back(UID_JPEGProcess1TransferSyntax);

    // configure SCU 
    scu.addPresentationContext(UID_CTImageStorage, ts); 
    scu.addPresentationContext(UID_MRImageStorage, ts); 
    scu.addPresentationContext(UID_DigitalXRayImageStorageForPresentation, xfer); 
    scu.addPresentationContext(UID_UltrasoundMultiframeImageStorage, xfer2);

    /* Initialize network */ 
    
    result = scu.initNetwork(); 
    if (result.bad())
        throw "Network initialization failed!";
    
    OFCondition status;
    
    
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
    result = scu.sendSTORERequest(0, /*"../DICOM_Images/testtext.dcm"*/ 0,/*0*/data, rspStatusCode );;
    if (result.bad()){   
        status = data->saveFile("../DICOM_Images/archive_1.dcm");
        if (status.bad())
            throw "Failed to save file after failure of C-STORE Request!";
        }

    //Second attempt for successful C-STORE association

    /*Extracting data from dicom file.*/ 
    DcmFileFormat dfile2;
    result = dfile2.loadFile("../DICOM_Images/1-01.dcm");
    if (result.bad())
        throw "Failed to load Dicom File!";
    DcmDataset *data2 = dfile2.getDataset();
    if (data2 == NULL)
        throw "Empty dataset!";
    

    //Assemble and send C-STORE request. Check if C-STORE was successful.
    rspStatusCode = 0;
    result = scu.sendSTORERequest(0, /*"../DICOM_Images/1-01.dcm"*/ 0,/*0*/data2, rspStatusCode );;
    if (result.bad()){   
        status = data2->saveFile("../DICOM_Images/archive_2.dcm");
        if (status.bad())
            throw "Failed to save file after failure of C-STORE Request!";
        }
        

    //Release association. 
    result = scu.releaseAssociation();
    if (result.bad())
        throw "Association release failed!";


    /*Request shutdown and stop listening. */ 
    pool.request_stop();
    //pool.join();
    
    }
    
 
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    
    catch(const char* txtError){
        cerr << "error: " << txtError << "\n";
    }
    

    catch(...){
        cerr << "Exception of unknown type!\n";
    }
    

return 0;
}



