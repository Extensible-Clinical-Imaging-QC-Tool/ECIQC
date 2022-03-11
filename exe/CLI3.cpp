#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include <dcmtk/config/osconfig.h>
#include "dcmtk/ofstd/ofstring.h"
#include "../src/communication/sender.hpp"
//#include "communication/receiver.hpp"
//#include "conductor.hpp"
#include <exception>

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
namespace po = boost::program_options;

int main(int argc, char** argv){

Uint8 ReceiverPortNumber;
OFString ReceiverPortName;
OFString ReceiverAETitle;
Uint8 SenderPortNumber;
OFString SenderPortName;
OFString SenderAETitle;

try {
    po::options_description ECIQC("Options");
    ECIQC.add_options()
        ("help", "produce help message")
        ("SenderAETitle", po::value<OFString>(&SenderAETitle)->default_value("TEST-SCU"), "set Sender AE Title")
        ("SenderPortNumber", po::value<Uint8>(&SenderPortNumber)->default_value(11112), "set Sender Port Number")
        ("SenderPortName", po::value<OFString>(&SenderPortName)->default_value("localhost"),"set Sender Port Name")
        ("ReceiverAETitle", po::value<OFString>(&ReceiverAETitle)->default_value("MOVESCP"), "set Receiver AE Title")
        ("ReceiverPortNumber", po::value<Uint8>(&ReceiverPortNumber)->default_value(104), "set Receiver Port Number")
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
                 << vm["SenderPortNumber"].as<Uint8>() << ".\n";
        } else {
            cout << "Sender Port Number was not set.\n";
        }
    
    if (vm.count("SenderPortName")) {
            cout << "Sender Port Name was set to " 
                 << vm["SenderPortName"].as<OFString>() << ".\n";
        } else {
            cout << "Sender Port Name was not set.\n";
        }
    if (vm.count("RecieverPortNumber")) {
            cout << "Receiver Port Number was set to " 
                 << vm["ReceiverPortNumber"].as<Uint8>() << ".\n";
        } else {
            cout << "Receiver Port Number was not set.\n";
        }
    if (vm.count("ReceiverPortName")) {
            cout << "Receiver Port Name was set to " 
                 << vm["ReceiverPortName"].as<OFString>() << ".\n";
        } else {
            cout << "Receiver Port Name was not set.\n";
        }   

    
    //Execute C-ECHO Request with Receiver
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
    //Negotiate association 
    result = scu.negotiateAssociation();

    //Check whether the server is listening//
    result = scu.sendECHORequest(0);
    //Release association 
    result = scu.releaseAssociation();
    

}
    
 
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }

    catch(...){
        cerr << "Exception of unknown type!\n";
    }

return 0;
}



