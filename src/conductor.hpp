#ifndef CONDUCTOR_H
#define CONDUCTOR_H
// #include
#include "parsing/Parser.hpp"
#include "communication/receiver.hpp"
/* #include "Sender.hpp" */

class Conductor {
public:
  /** Constructor */
  Conductor(/*OFString cfp1, OFString cfp2,*/ Uint8 rPortNum, OFString rPortName,
            Uint8 sPortNum, OFString sPortName);
  void setOptional();
  // Receiver

  void run();


  /** members */
  Receiver r;
  //Parser p1;
  //Parser p2;
  /* Sender s; */
  OFString sPortNum, sPortName, qPortNum, qPortName;
  /** functions */

  // Parser

  // Metadata Editor

  // Validator

  // Sender
};

#endif