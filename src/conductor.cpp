// #include
#include "conductor.hpp"
#include <dcmtk/config/osconfig.h>
#include <iostream>

/* In between command line and constructor, have something that
separates the compulsory and optional variables. Constructor only
takes compulsory variables. Optional variables are then fed to
specific setX() functions */

// Constructor ()
// 2 config files, 2*(port number and port name)

Conductor::Conductor(OFString cfp1, OFString cfp2, Uint8 rPortNum,
                     OFString rPortName, Uint8 sPortNum, OFString sPortName)
    : p1(cfp1), p2(cfp2), r(rPortNum, rPortName)
      /*sender(PortNum, PortName)*/ 
      {}

// set optional variables
void Conductor::setOptional(/*all optional variables */) {
  /* setStorage() */
}

// Receiver




// Parser

// Metadata Editor

// Validator

// Sender

void Conductor::run() {
    // Receiver starts listening
    DcmDataset dset;
    DcmDataset* pDset = &dset;

    r.setpooldataset(pDset);
    r.start();


    // Validation Parser
    OFString mode = 'val';
    p1.setDicomFile(pDset);
    OFBool pass = p1.pRun(mode);
    s(qPortNum, qPortName);
    // ME Parser
    if(pass) {
      mode = 'me';
      p2.setDicomFile(pDset);
      p2.pRun(mode);

      s(sPortNum, sPortName);
    }


} 
