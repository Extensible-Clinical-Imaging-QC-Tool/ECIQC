#ifndef SENDER_H 
#define SENDER_H 

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */ 

#include "dcmtk/dcmnet/dstorscu.h"     /* Covers most common dcmdata classes */ 

class Sender : public DcmStorageSCU 
{ 

public: 
 /**Constructor. */
  Sender(std::string ae_title, std::string peer_hostname, Uint16 peer_port, std::string peer_aetitles);

  OFCondition send(DcmDataset& dataset);
  OFCondition send_file(const std::string& filename);
  OFCondition send_echo();

};

 



 

#endif // TESTSCU_H 
