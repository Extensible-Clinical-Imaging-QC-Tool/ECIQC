#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include <dcmtk/config/osconfig.h>
#include "dcmtk/ofstd/ofstring.h"
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
Uint16 SenderPortNumber;
OFString SenderPortName;
OFString SenderAETitle;

try {
    po::options_description ECIQC("Options");
    ECIQC.add_options()
        ("help", "produce help message")
        ("SenderAETitle", po::value<OFString>(&SenderAETitle)->default_value("TEST-SCU"), "set Sender AE Title")
        ("SenderPortNumber", po::value<Uint16>(&SenderPortNumber)->default_value(11112), "set Sender Port Number")
        ("SenderPortName", po::value<OFString>(&SenderPortName)->default_value("localhost"),"set Sender Port Name")
        ("ReceiverAETitle", po::value<OFString>(&ReceiverAETitle)->default_value("MOVESCP"), "set Receiver AE Title")
        ("ReceiverPortNumber", po::value<Uint16>(&ReceiverPortNumber)->default_value(104), "set Receiver Port Number")
        ("ReceiverPortName", po::value<OFString>(&ReceiverPortName)->default_value("www.dicomserver.co.uk"), "set Receiver Port Name");
        
        
    po::variables_map vm;
    //po::store(po::parse_command_line(argc, argv, ECIQC), vm); // use this if want user to enter configuration settings as arguments from command line directly
    po::store(po::parse_command_line(argc, argv, ECIQC),vm); 
    po::notify(vm);

    if (vm.count("help")) {
        cout << ECIQC << "\n";
        return 0;
      }
    
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
        throw "Association negatiation failed!";

    //Check whether the server is listening//
    result = scu.sendECHORequest(0);
    if (result.bad())
        throw "Send ECHO Request failed!";
    //Release association 
    result = scu.releaseAssociation();
    if (result.bad())
        throw "Association Released failed!";

    //Execute C-STORE Request with SCU
    OFshared_ptr<OFList<DcmDataset>>  pt(new OFList<DcmDataset>);
    Receiver pool(ReceiverPortNumber, ReceiverAETitle);
    pool.setpointer(pt);  
    
    // Define presentation contexts, propose all uncompressed transfer syntaxes 
    ts.push_back(UID_LittleEndianExplicitTransferSyntax); 
    ts.push_back(UID_BigEndianExplicitTransferSyntax); 
    ts.push_back(UID_LittleEndianImplicitTransferSyntax); 

    // Define a separate transfer syntax needed for the X-ray image
    OFList<OFString> xfer;
    xfer.push_back(UID_LittleEndianImplicitTransferSyntax);

    //Start listening
    pool.start();

    // configure SCU 
    scu.setAETitle(SenderAETitle); 
    scu.setPeerHostName(ReceiverPortName); 
    scu.setPeerPort(ReceiverPortNumber); 
    scu.setPeerAETitle(ReceiverAETitle);
    scu.setVerbosePCMode(OFTrue);
    scu.addPresentationContext(UID_CTImageStorage, ts); 
    scu.addPresentationContext(UID_MRImageStorage, ts); 
    scu.addPresentationContext(UID_DigitalXRayImageStorageForPresentation, xfer);
    scu.addPresentationContext(UID_VerificationSOPClass, ts); 

    /* Initialize network */ 
    result = scu.initNetwork(); 
    if (result.bad())
        throw 'Network initialization failed!';

    OFCondition status = scu.addDicomFile("../DICOM_Images/testtext.dcm", ERM_fileOnly,false);
    if (status.bad())
        throw 'Failed to add Dicom File!';

    /*Extracting data from dicom file.*/ 
    DcmFileFormat dfile;
    result = dfile.loadFile("../DICOM_Images/testtext.dcm");
    if (result.bad())
        throw 'Failed to load Dicom File!';
    DcmDataset *data = dfile.getDataset();
    if (data == NULL)
        throw 'Empty dataset!';


    /* Negotiate Association */ 
    result = scu.negotiateAssociation(); 
    if (result.bad())
        throw 'Association negotiation failed!';

    /*Assemble and send C-STORE request. Check if C-STORE was successful.*/
    Uint16 rspStatusCode = 0;
    result = scu.sendSTORERequest(0, /*"../DICOM_Images/1-01.dcm"*/ 0,/*0*/data, rspStatusCode = 0);;
    if (result.bad()){   
        status = data->saveFile("../DICOM_Images/archive_1.dcm");
        if (status.bad())
            throw 'Failed to save file after failure of C-STORE Request!';
        }
        

    /*Release association. */
    result = scu.releaseAssociation();
    if (result.bad())
        throw 'Association release failed!';

    /*Request shutdown and stop listening. */ 
    pool.request_stop();
    pool.join();
    

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



