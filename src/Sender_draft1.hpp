#ifndef SENDER_H_
#define SENDER_H_

#include <iostream>
#include <dcmtk/ofstd/ofstd.h> 
#include <dcmtk/dcmnet/dstorscu.h> 

class Sender {

    public:

    /** Constructor. */
    //Sender();

    OFBool Send_Dicom_Files(const char* opt_peer, const char* opt_peerTitle, const char* dicom_dir);
    
    OFCondition result;

    


};

#endif // SENDER_H_