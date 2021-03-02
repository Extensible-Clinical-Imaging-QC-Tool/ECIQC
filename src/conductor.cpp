// #include
#include <iostream>
#include <dcmtk/config/osconfig.h>
#include "conductor.hpp" 


//Constructor
Conductor::Conductor()
    : m_receiver()
{

}
// Receiver
void Conductor::setReceiver(Uint16 port, OFString ae_title)
{
    m_receiver.setportnumber(port);
    m_receiver.setaetitle(ae_title);
}


// Parser 

    // Metadata Editor


// Validator 


// Sender