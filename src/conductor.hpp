#ifndef CONDUCTOR_H
#define CONDUCTOR_H
// #include
#include "communication/receiver.hpp"
#include "communication/sender.hpp"
#include "parsing/Parser.hpp"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include <signal.h>

class Conductor : public OFThread {
public:
  /** Constructor */
  Conductor(  std::string SenderAETitle, std::string PeerAETitle,  Uint16 PeerPortNumber, std::string PeerPortName,
                     std::string ReceiverAETitle, Uint16 ReceiverPortNumber);
            
            
  
  void setOptional();
  
  void run();
  void pipeline(DcmDataset& dataset);

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
  OFshared_ptr<OFList<DcmDataset>>  m_received_pDset;

private:
  static void sig_handler(sig_atomic_t);
  static volatile sig_atomic_t m_keep_running;
 
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
