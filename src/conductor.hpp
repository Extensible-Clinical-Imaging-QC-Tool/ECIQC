#ifndef CONDUCTOR_H
#define CONDUCTOR_H
// #include
//#include "parsing/Parser.hpp"
#include "communication/receiver.hpp"
#include "communication/sender.hpp"

class Conductor {
public:
  /** Constructor */
  Conductor( std::string SenderAETitle, Uint16 SenderPortNumber, std::string SenderPortName, 
            std::string ReceiverAETitle, Uint16 ReceiverPortNumber, std::string ReceiverPortName);
            
  
  void setOptional();
  // Receiver

  void run();


  /** members */
  Receiver scp();
  Sender scu();
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