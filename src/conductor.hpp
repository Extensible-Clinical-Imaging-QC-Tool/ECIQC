#ifndef CONDUCTOR_H
#define CONDUCTOR_H
// #include 
#include "draft_receiver.hpp"

class Conductor
{
 public:
    /** Constructor */
    Conductor();
    
    // Receiver

    /** members */
    Receiver m_receiver;
    

    /** functions */
    void setReceiver(Uint16 port, OFString ae_title);


// Parser 

    // Metadata Editor


// Validator 


// Sender

};

#endif