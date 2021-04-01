#include <limits>
#include <type_traits>
#include <iostream>
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <dcmtk/dcmnet/scu.h>
#include "dcmtk/dcmnet/scp.h"
#include <dcmtk/dcmnet/dstorscu.h>

#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftimer.h"

#include "../src/Exception.hpp"
#include "../src/MyLibrary.hpp"

#include "../src/Sender_draft1.hpp"

using namespace cpp_template;

/* Set parameters */

//our application entity title used for calling the peer machine 
#define APPLICATIONTITLE     "TEST-SCU" 

// host name of the peer machine 
#define PEERHOSTNAME         "www.dicomserver.co.uk" 

// TCP/IP port to connect to the peer machine 
#define PEERPORT 11112 

// application entity title of the peer machine 
#define PEERAPPLICATIONTITLE "MOVESCP" 

// MOVE destination AE Title 
#define MOVEAPPLICATIONTITLE "TEST-SCU" 


TEST_CASE("Test the Sender Class"){

  std::string s = std::to_string(PEERPORT); 
  const char* opt_peer = s.c_str(); 
  const char* opt_peerTitle = PEERAPPLICATIONTITLE;
  const char *dicom_dir = "../DICOM_Images/";

  Sender object;
  CHECK(object.Send_Dicom_Files(opt_peer,opt_peerTitle,dicom_dir));

}


//#endif
