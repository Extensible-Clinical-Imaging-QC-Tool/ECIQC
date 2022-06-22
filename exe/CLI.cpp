#include <iostream>
#include <iterator>
#include <vector>
#include <exception>
#include <boost/program_options.hpp>
#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/oflog/fileap.h"
//#include "../src/communication/sender.hpp"
//#include "../src/communication/receiver.hpp"
#include "conductor.hpp"


using std::cerr;
using std::cout;
using std::endl;
using std::exception;
namespace po = boost::program_options;




int main(int argc, char** argv){

//Variables that will store parsed values.
Uint16 ReceiverPortNumber;
std::string ReceiverPortName;
std::string ReceiverAETitle;
Uint16 SenderPortNumber;
std::string SenderPortName;
std::string SenderAETitle;


//Setup options
try {
    po::options_description ECIQC("Options");
    ECIQC.add_options()
        ("help", "produce help message")
        ("SenderAETitle", po::value<std::string>(&SenderAETitle)->default_value("TestSCU"), "set Sender AE Title")
        ("SenderPortNumber", po::value<Uint16>(&SenderPortNumber)->default_value(104), "set Sender Port Number")
        ("SenderPortName", po::value<std::string>(&SenderPortName)->default_value("localhost"),"set Sender Port Name")
        ("ReceiverAETitle", po::value<std::string>(&ReceiverAETitle)->default_value("TestSCP"), "set Receiver AE Title")
        ("ReceiverPortNumber", po::value<Uint16>(&ReceiverPortNumber)->default_value(11112), "set Receiver Port Number")
        ("ReceiverPortName", po::value<std::string>(&ReceiverPortName)->default_value("localhost"), "set Receiver Port Name");
        
        
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, ECIQC),vm); 
    po::notify(vm);

    if (vm.count("help")) {
        cout << ECIQC << "\n";
        return 0;
      }

     if (vm.count("SenderAETitle")) {
            cout << "Sender Application Entity Title was set to " 
                 << vm["SenderAETitle"].as<std::string >() << ".\n" ;
        } else {
            cout << "Sender Application Entity Title was not set.\n";
        } 
    
    if (vm.count("SenderPortNumber")) {
            cout << "Sender Port Number was set to " 
                 << vm["SenderPortNumber"].as<Uint16>() << ".\n";
        } else {
            cout << "Sender Port Number was not set.\n";
        }
    
    if (vm.count("SenderPortName")) {
            cout << "Sender Port Name was set to " 
                 << vm["SenderPortName"].as<std::string>() << ".\n";
        } else {
            cout << "Sender Port Name was not set.\n";
        }
    

    if (vm.count("ReceiverAETitle")) {
            cout << "Receiver Application Entity Title was set to " 
                 << vm["ReceiverAETitle"].as<std::string>() << ".\n";
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
                 << vm["ReceiverPortName"].as<std::string>() << ".\n";
        } else {
            cout << "Receiver Port Name was not set.\n";
        } 
  

    //Specify log pattern in .log file.
    
    OFunique_ptr<dcmtk::log4cplus::Layout> layout(new dcmtk::log4cplus::PatternLayout("%D{%Y-%m-%d %H:%M:%S.%q} %5p: %m%n"));
    /* Denote that a log file should be used that is appended to. The file is re-created every
        time the code gets to this point.
    */
    dcmtk::log4cplus::SharedAppenderPtr logfile(new dcmtk::log4cplus::FileAppender("CLI.log"));
    //logfile->setLayout(OFmove(layout));

    /* make sure that only the file logger is used */
    dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
    log.removeAllAppenders();
    log.addAppender(logfile);
    log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL);
    
    
    //Conductor
    Conductor conductor(SenderAETitle, SenderPortNumber, SenderPortName, ReceiverAETitle,ReceiverPortNumber, ReceiverPortName);
    conductor.run();

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


