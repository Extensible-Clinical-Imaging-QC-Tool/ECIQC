#ifndef CONDUCTOR_H
#define CONDUCTOR_H
// #include
#include "communication/receiver.hpp"
#include "communication/sender.hpp"
#include "parsing/Parser.hpp"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

class Conductor {
public:
  /** Constructor */
  Conductor(  std::string SenderAETitle, std::string PeerAETitle,  Uint16 PeerPortNumber, std::string PeerPortName,
                     std::string ReceiverAETitle, Uint16 ReceiverPortNumber);
            
            
  
  void setOptional();
  

  void run();

  ~Conductor();
  /** members */
  Receiver scp;
  Sender scu;
  std::string SenderAETitle;
  std::string PeerAETitle;
  Uint16 PeerPortNumber;
  std::string PeerPortName;
  std::string ReceiverAETitle;
  Uint16 ReceiverPortNumber;
 
  //Parser p1;
  //Parser p2;
  /* Sender s; */
  //std::string sPortNum, sPortName, qPortNum, qPortName;
  /** functions */

  // Parser

  // Metadata Editor

  // Validator

  // Sender
};

#endif