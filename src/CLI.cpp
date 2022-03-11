#include <iostream>
#include <iterator>
#include <vector>
#include <boost/program_options.hpp>
#include <dcmtk/config/osconfig.h>
#include "conductor.hpp"
#include <exception>

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
namespace po = boost::program_options;

int main(int argc, char** argv){

OFString Configfile1= "schema.json";
OFString Configfile2= "schema.json"; //validator config file
int ReceiverPortNumber = 11112;
OFString ReceiverPortName = "TestSCP";
int SenderPortNumber = 11118;
OFString SenderPortName = "TestSCU";
//uint8_t SenderQuarantinePortNumber;
//std::string SenderQuarantinePortName;

    try {
        std::ifstream Config_File("communication/Config.ini");
        po::options_description ECIQC("Options");
        ECIQC.add_options()
            ("help", "produce help message");
            //("Configfile1", value<std::string>(&Configfile1)->default_value("schema.json"));
            //("Configfile2", value<std::string>(&Configfile2)->default_value("schema.json"));
            ("ReceiverPortNumber", po::value<int>(&ReceiverPortNumber)->default_value(11112));
            ("ReceiverPortName", po::value <int> (&ReceiverPortName)->default_value("TestSCP"));
            //("SenderPortNumber", value<uint8_t>(&SenderPortNumber)->default_value(11118));
            //("SenderPortName", value<std::string>(&SenderPortName)->default_value("TestSCU"));
            //("SenderQuarantinePortNumber", value<uint8_t>(&SenderQuarantinePortNumber)->default_value(00001));
            //("SenderQuarantinePortName", value<std::string>(&SenderQuarantinePortName)->default_value("TestQSCU"));
        

        po::variables_map vm;
        //po::store(po::parse_command_line(argc, argv, ECIQC), vm); // use this if want user to enter configuration settings as arguments from command line directly
        store(parse_config_file(Config_File, ECIQC), vm); // parse configuration settings from "Config.ini" file
        po::notify(vm);

        if (vm.count("help")) {
            cout << ECIQC << "\n";
            return 0;
        }
        
        if (vm.count("Configfile1"))
        {
            cout << Configfile1;
        }

        //Execute ECIQC tool
        Conductor obj(Configfile1, Configfile2, ReceiverPortNumber, ReceiverPortName, SenderPortNumber, SenderPortName); 
    }
    
        catch(exception& e) {
            cerr << "error: " << e.what() << "\n";
            return 1;
        }

    return 0;
    
}


