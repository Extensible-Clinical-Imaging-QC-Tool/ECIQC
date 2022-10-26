#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#include "communication/receiver.hpp"
#include "communication/sender.hpp"
#include <dcmtk/dcmdata/dcdatset.h>

class Conductor {
  Sender m_destination;
  Sender m_quarentine;
  Receiver m_receiver;
  OFshared_ptr<ThreadSafeQueue<DcmDataset>> m_todo;

public:
  Conductor() = default;
  ~Conductor();
  Conductor(const Conductor &) = delete;
  Conductor(Conductor &&) = delete;
  Conductor &operator=(const Conductor &) = delete;
  Conductor &operator=(Conductor &&) = delete;
  void setup_receiver(const std::string &aetitle, int port);
  void setup_destination(const std::string &aetitle,
                         const std::string &hostname, int port);
  void setup_quarentine(const std::string &aetitle, const std::string &hostname,
                        int port);
  void finalise_initialisation();
  void process_next_dataset();

private:
  void process_dataset(DcmDataset &dataset);
};

#endif
