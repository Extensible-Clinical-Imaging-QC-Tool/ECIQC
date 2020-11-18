#include <boost/program_options.hpp> //include boost progam option to use flags
namespace po = boost::program_options;

#include <iostream>

int main(int ac, char* av[])
{
    po::options_description desc("Allowed options"); // create container for options
    desc.add_options()
        ("help,h", "using --help or -H produces this help message.");

    po::variables_map vm; //create variable_maps class vm to store command line flags 
    po::store(po::parse_command_line(ac, av, desc), vm); // parse the command line put and store flags in vm 
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    return 0;
}