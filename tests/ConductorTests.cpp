#include <limits>
#include <type_traits>
#include <iostream>
#include "catch.hpp"


#include <dcmtk/config/osconfig.h>
#include "../src/conductor.hpp"

#ifdef WITH_THREADS

// Receiver Tests

TEST_CASE("Test Conductor config", "[CT]")
{
    Conductor conductor("TestSCU", 104, "localhost","TestSCP", 11112, "localhost");
    //conductor.scp;
    conductor.run();
    CHECK(conductor.scp.getConfig().getAETitle() == "TestSCP");
    CHECK(conductor.scu.getAETitle() == "TestSCU");
}



#endif