#include <iostream>
#include <iterator>
#include <vector>
#include <exception>
#include <boost/program_options.hpp>
#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/oflog/fileap.h"
#include "../src/communication/sender.hpp"
#include "../src/communication/receiver.hpp"
#include <stdexcept>

#ifdef WITH_THREADS

#include <dcmtk/dcmpstat/dcmpstat.h>
#include <dcmtk/dcmnet/scu.h>
#include <dcmtk/dcmnet/dstorscu.h>
//#include "conductor.hpp"

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
//Uint16 SenderPortNumber;
//std::string SenderPortName;
//std::string SenderAETitle;

//Setup options
try {
    po::options_description ECIQC("Options");
    ECIQC.add_options()
        ("help", "produce help message")
        //("SenderAETitle", po::value<std::string>(&SenderAETitle)->default_value("TestSCU"), "set Sender AE Title")
        //("SenderPortNumber", po::value<Uint16>(&SenderPortNumber)->default_value(104), "set Sender Port Number")
        //("SenderPortName", po::value<std::string>(&SenderPortName)->default_value("localhost"),"set Sender Port Name")
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
    dcmtk::log4cplus::SharedAppenderPtr logfile(new dcmtk::log4cplus::FileAppender("CLI4.log"));
    //logfile->setLayout(OFmove(layout));

    /* make sure that only the file logger is used */
    dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
    log.removeAllAppenders();
    log.addAppender(logfile);
    log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL);


 // Create a test SCU (sender) to send C-ECHO request
  struct TestSCU : DcmSCU, OFThread {
      OFCondition result;
    protected:
      void run(){
        negotiateAssociation();
        result = sendECHORequest(0);
        releaseAssociation();
        }
    };


//Instantiate receiver 

 Receiver pool(ReceiverPortNumber, ReceiverPortName);

// Start listening
pool.start();


  // Define presentation contexts for SCU
  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

  // Configure SCU and initialize
  OFVector<TestSCU*> scus(2);
  for (OFVector<TestSCU*>::iterator it1 = scus.begin(); it1 != scus.end(); ++it1)
      {
          *it1 = new TestSCU;
          (*it1)->setAETitle("PoolTestSCU");
          (*it1)->setPeerAETitle("TestSCP");
          (*it1)->setPeerHostName("localhost");
          (*it1)->setPeerPort(11112);
          (*it1)->addPresentationContext(UID_VerificationSOPClass, xfers);
          (*it1)->initNetwork();
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
            throw std::invalid_argument("Association was not succesfful");
        delete *it3;
      };
  
  // Request shutdown.
  pool.request_stop();
  pool.join();
}
 
     catch(const char* txtError){
        cerr << "error: " << txtError << "\n";
    }
    
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
return 0;

}

#endif