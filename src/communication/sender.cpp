/**
 * @file test_scu.cpp
 * @brief Example code on how to program an actual DICOM client with the DcmStoreSCU class. 
 * @version 0.1
 * @date 2022-03-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */ 
#include "sender.hpp"
#include "dcmtk/dcmnet/diutil.h" 

/**Constructor.*/
Sender::Sender(OFString ae_title, OFString peer_hostname, Uint16 peer_port, OFString peer_aetitle){}


/**Destructor .*/
Sender::~Sender(){}

// ---------------------------------------------------------------------------

/*Send DICOM dataset.*/
void Sender::run()
{

}

