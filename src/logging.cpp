#include "logging.hpp"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/oflog/fileap.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/oflog/logger.h"
#include "dcmtk/oflog/oflog.h"

void set_root_logging(const std::string& filename, bool debug) {
    // Specify log pattern in .log file
OFunique_ptr<dcmtk::log4cplus::Layout> layout(
    new dcmtk::log4cplus::PatternLayout(
        "%D{%Y-%m-%d %H:%M:%S.%q} %5p: %m%n"));
/* Denote that a log file should be used that is appended to. The file is
    re-created every time the code gets to this point.
*/
dcmtk::log4cplus::SharedAppenderPtr test_log_file(
    new dcmtk::log4cplus::FileAppender(filename.c_str()));
// logfile->setLayout(OFmove(layout));

/* make sure that only the file logger is used */
dcmtk::log4cplus::Logger test_log = dcmtk::log4cplus::Logger::getRoot();
test_log.removeAllAppenders();
test_log.addAppender(test_log_file);
if (debug) {
  test_log.setLogLevel(OFLogger::TRACE_LOG_LEVEL);
} else {
  test_log.setLogLevel(OFLogger::INFO_LOG_LEVEL);
}
}




void set_logging(const std::string& filename, bool debug) {
    // Specify log pattern in .log file
OFunique_ptr<dcmtk::log4cplus::Layout> layout(
    new dcmtk::log4cplus::PatternLayout(
        "%D{%Y-%m-%d %H:%M:%S.%q} %5p: %m%n"));
/* Denote that a log file should be used that is appended to. The file is
    re-created every time the code gets to this point.
*/
dcmtk::log4cplus::SharedAppenderPtr test_log_file(
    new dcmtk::log4cplus::FileAppender(filename.c_str()));
// logfile->setLayout(OFmove(layout));

/* make sure that only the file logger is used */
OFLogger test_log = get_logger();
test_log.removeAllAppenders();
test_log.addAppender(test_log_file);
if (debug) {
  test_log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL);
} else {
  test_log.setLogLevel(OFLogger::INFO_LOG_LEVEL);
}
}

OFLogger get_logger() {
    return dcmtk::log4cplus::Logger::getInstance("qctool");
}