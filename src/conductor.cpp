// #include
#include <iostream>
#include <dcmtk/config/osconfig.h>
#include "conductor.hpp" 


//Constructor
Conductor::Conductor(Uint16 port, OFString ae_title)
        : receiver(port, ae_title)
{
    //Receiver receiver;
    //receiver.setportnumber(port);
    //receiver.setaetitle(ae_title);
}
// Receiver
/*void Conductor::setReceiver(Uint16 port, OFString ae_title)
{
    receiver.setportnumber(port);
    receiver.setaetitle(ae_title);
} */


// Parser 

    // Metadata Editor


// Validator 


// Sender