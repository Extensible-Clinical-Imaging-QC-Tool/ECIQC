#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#include "communication/Receiver.hpp"
#include "communication/Sender.hpp"
#include "parsing/Parser.hpp"
#include <dcmtk/dcmdata/dcdatset.h>


class Conductor {
  Parser m_parser;
  Sender m_destination;
  Sender m_quarentine;
  Receiver m_receiver;
  OFshared_ptr<ThreadSafeQueue<DcmDataset>> m_todo;

public:
  explicit Conductor(std::istream &json_config);
  ~Conductor() = default;
  Conductor(const Conductor &) = delete;
  Conductor(Conductor &&) = delete;
  Conductor &operator=(const Conductor &) = delete;
  Conductor &operator=(Conductor &&) = delete;
  void process_next_dataset();
  void shutdown_receiver();
  
private:
  void setup_parser(const json &config);
  void setup_receiver(const std::string &aetitle, int port);

  void setup_destination(const std::string &aetitle,
                         const std::string &peer_aetitle,
                         const std::string &hostname, const int port);
  void setup_quarentine(const std::string &aetitle,
                        const std::string &peer_aetitle,
                        const std::string &hostname, const int port);
  void initialise(const std::string &json_config);
  void process_dataset(DcmDataset &dataset);
};

#endif
