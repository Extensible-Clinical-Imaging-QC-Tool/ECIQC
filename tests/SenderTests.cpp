#include <limits>
#include <type_traits>
#include <iostream>
#include "catch.hpp"


#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <dcmtk/dcmnet/scu.h>
#include <dcmtk/dcmnet/dstorscu.h>



#include "../src/Exception.hpp"
#include "../src/MyLibrary.hpp"

#include "../src/Sender.hpp"

using namespace std;

// This tests basic configuration of the Receiver.

Sender config;


TEST_CASE("Test Sender Configuration"){



}