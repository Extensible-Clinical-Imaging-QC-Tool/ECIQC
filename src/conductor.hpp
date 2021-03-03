#ifndef CONDUCTOR_H
#define CONDUCTOR_H
// #include 
#include "draft_receiver.hpp"

class Conductor
{
 public:
    /** Constructor */
    Conductor(Uint16 port, OFString ae_title);
    
    // Receiver

    /** members */
    Receiver receiver;
    

    /** functions */
    //void setReceiver(Uint16 port, OFString ae_title);


// Parser 

    // Metadata Editor


// Validator 


// Sender

};

#endif