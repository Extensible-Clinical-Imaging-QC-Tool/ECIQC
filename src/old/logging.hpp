#include <string>
#include "dcmtk/oflog/oflog.h"


void set_root_logging(const std::string& filename, bool debug);
void set_logging(const std::string& filename, bool debug);
OFLogger get_logger();

