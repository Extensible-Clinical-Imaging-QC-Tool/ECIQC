#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#include "communication/sender.hpp"
#include "communication/receiver.hpp"

void pipeline(Sender &destination, Sender &quarentine, DcmDataset &dataset);
void conductor(Sender &destination, Sender &quarentine, Receiver &receiver);
 


#endif
