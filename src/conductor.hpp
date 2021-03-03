#ifndef CONDUCTOR_H
#define CONDUCTOR_H
// #include 
#include "draft_receiver.hpp"
#include "Parser.hpp"

class Conductor
{
 public:
    /** Constructor */
    Conductor(Uint16 port, OFString ae_title);
    
    // Receiver

    /** members */
    Receiver receiver;
    Parser p1, p2;
    

    /** functions */
    //void setReceiver(Uint16 port, OFString ae_title);


// Parser 

    // Metadata Editor


// Validator 

OFString config1;
OFString config2;
OFString port_no;

// Sender

};

#endif