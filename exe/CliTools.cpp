#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>

int main(int ac, char* av[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    return 0;
}