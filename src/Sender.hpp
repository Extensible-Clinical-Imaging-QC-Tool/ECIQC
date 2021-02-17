#ifndef SENDER_H_
#define SENDER_H_

#include <iostream>
#include <dcmtk/ofstd/ofstd.h> 
#include <dcmtk/dcmnet/dstorscu.h> 

class Sender {

    public:
    Sender(OFString file_path);
};

#endif // SENDER_H_