// #include
#include <iostream>
#include <dcmtk/config/osconfig.h>
#include "conductor.hpp" 

/* In between command line and constructor, have something that
separates the compulsory and optional variables. Constructor only
takes compulsory variables. Optional variables are then fed to
specific setX() functions */


//Constructor ()
// 2 config files, 2*(port number and port name)

Conductor::Conductor(OFString cfp1,
                     OFString cfp2,
                     Uint8 rPortNum,
                     OFString rPortName,
                     Uint8 sPortNum,
                     OFString sPortName)
    : p1(cfp1), p2(cfp2), receiver(rPortNum, rPortName), 
    sender(rPortNum, rPortName)
{

}

// set optional variables
void Conductor::setOptional(/*all optional variables */) {
    setStorage(/*blah*/)

    
}

// Receiver



// Parser 

    // Metadata Editor


// Validator 


// Sender