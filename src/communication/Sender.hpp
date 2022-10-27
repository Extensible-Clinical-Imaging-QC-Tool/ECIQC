#ifndef SENDER_H
#define SENDER_H

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/dstorscu.h" /* Covers most common dcmdata classes */

class Sender : public DcmStorageSCU {

public:
  /**Constructor. */
  Sender();
  Sender(const Sender &) = delete;
  Sender(Sender &&) = delete;
  Sender &operator=(const Sender &) = delete;
  Sender &operator=(Sender &&) = delete;
  Sender(std::string aetitle, std::string peer_hostname, Uint16 peer_port,
         std::string peer_aetitle);

  void set_aetitle(const std::string &title);
  void set_peer_port( int port);
  void set_peer_hostname(const std::string &hostname);
  void set_peer_aetitle(const std::string &title);

  OFCondition send(DcmDataset &dataset);
  OFCondition send_file(const std::string &filename);
  OFCondition send_echo();
};

#endif // TESTSCU_H
