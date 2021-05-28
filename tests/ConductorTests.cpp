#include <limits>
#include <type_traits>
#include <iostream>
#include "catch.hpp"


#include <dcmtk/config/osconfig.h>
#include "../src/conductor.hpp"

#ifdef WITH_THREADS

// Receiver Tests

TEST_CASE("Test Conductor config")
{
    Conductor conductor;
    conductor.setReceiver(104, "SABSTool");
    CHECK(conductor.receiver.getConfig().getPort() == 104);
    CHECK(conductor.receiver.getConfig().getAETitle() == "SABSTool");

}



#endif