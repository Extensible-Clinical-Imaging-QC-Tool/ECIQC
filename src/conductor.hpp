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
  Conductor( std::string SenderAETitle, Uint16 SenderPortNumber, std::string SenderPortName, 
            std::string ReceiverAETitle, Uint16 ReceiverPortNumber, std::string ReceiverPortName);
            
            
  
  void setOptional();
  

  void run();

  ~Conductor();
  /** members */
  Receiver scp;
  Receiver testSCP;
  Sender scu;
  std::string SenderAETitle;
  Uint16 SenderPortNumber;
  std::string SenderPortName;
  std::string ReceiverAETitle;
  Uint16 ReceiverPortNumber;
  std::string ReceiverPortName;
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