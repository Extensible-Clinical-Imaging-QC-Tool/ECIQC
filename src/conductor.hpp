#ifndef CONDUCTOR_H
#define CONDUCTOR_H
// #include
#include "parsing/Parser.hpp"
#include "communication/receiver.hpp"
/* #include "Sender.hpp" */

class Conductor {
public:
  /** Constructor */
  Conductor(/*std::string cfp1, std::string cfp2,*/ Uint8 rPortNum, std::string rPortName,
            Uint8 sPortNum, std::string sPortName);
  OFString rPortName = rPortName.c_str();
  OFString sPortName = sPortName.c_str();
  void setOptional();
  // Receiver

  void run();


  /** members */
  Receiver r;
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