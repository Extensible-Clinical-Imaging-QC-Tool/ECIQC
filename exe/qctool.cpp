#include "dcmtk/config/osconfig.h"
#include "dcmtk/oflog/fileap.h"
#include "dcmtk/ofstd/ofstring.h"
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <thread>         // std::this_thread::sleep_for

#include <chrono>
#include <csignal>
#include <exception>
#include <iostream>
#include <iterator>
#include <vector>
// #include "../src/communication/sender.hpp"
// #include "../src/communication/receiver.hpp"
#include "Conductor.hpp"
#include "logging.hpp"

namespace po = boost::program_options;

int main(int argc, char **argv) {

  // Variables that will store parsed values.
  std::string log_file;
  std::string config_file;
  bool debug = false;

  // Setup options
  try {
    po::options_description options("Allowed options");
    options.add_options()("help", "produce help message")
        ("log", po::value<std::string>(&log_file)->default_value("ECIQC.log"),
        "log filename")
        ("debug", po::value<bool>(&debug)->default_value(false),
        "output debug messages to logfile");

    po::options_description hidden;
    hidden.add_options()("config-file",
                         po::value<std::string>(&config_file)->required(),
                         "ECIQC.json");

    po::options_description all_options;
    all_options.add(options);
    all_options.add(hidden);

    po::positional_options_description poptions;
    poptions.add("config-file", 1);

    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv)
                  .options(all_options)
                  .positional((poptions))
                  .run(),
              var_map);

    if (var_map.count("help") != 0U) {
      std::cout << "Usage: qctool [options] <config-file>" << '\n';
      std::cout << options << "\n";
      return 0;
    }

    po::notify(var_map);

    set_root_logging("./result/pipeline_test.log", true);
    /*
    // Specify log pattern in .log file.
    OFunique_ptr<dcmtk::log4cplus::Layout> layout(
        new dcmtk::log4cplus::PatternLayout(
            "%D{%Y-%m-%d %H:%M:%S.%q} %5p: %m%n"));
    // Denote that a log file should be used that is appended to. The file is
    //   re-created every time the code gets to this point.
    
    dcmtk::log4cplus::SharedAppenderPtr logfile(
        new dcmtk::log4cplus::FileAppender(log_file.c_str()));
    // logfile->setLayout(OFmove(layout));

    // make sure that only the file logger is used
    dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
    log.removeAllAppenders();
    log.addAppender(logfile);
    if (debug) {
      log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL);
    } else {
      log.setLogLevel(OFLogger::INFO_LOG_LEVEL);
    }
    */


    std::ifstream configFile(config_file.c_str());
    if (!configFile) {
      throw std::runtime_error(std::string("Cannot read config file: ") + config_file);
    }

    Conductor conductor(configFile);
    while (true) {
      conductor.process_next_dataset();
    }
  }

  catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  catch (const char *txtError) {
    std::cerr << "Error: " << txtError << "\n";
  }

  catch (...) {
    std::cerr << "Exception of unknown type!\n";
  }

  return 0;
}
