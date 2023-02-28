#include "Conductor.hpp"
#include "communication/ThreadSafeQueue.hpp"
#include "memory"
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/ofstd/ofmem.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include "dcmtk/oflog/fileap.h"
#include "logging.hpp"
    
#include "../libs/nlohmann_json/single_include/nlohmann/json.hpp"

using json = nlohmann::json;

Conductor::Conductor(std::istream &json_config) {
  auto config = json::parse(json_config);
  setup_parser(config["metadata"]);
  setup_receiver(config["receiver"]["aetitle"], config["receiver"]["port"]);
  setup_destination(
      "QCtool Destination Sender", config["destination"]["aetitle"],
      config["destination"]["hostname"], config["destination"]["port"]);
  setup_quarentine(
      "QCtool Quarentine Sender", config["quarentine"]["aetitle"],
      config["quarentine"]["hostname"], config["quarentine"]["port"]);
}
 
void Conductor::setup_parser(const json &config) { m_parser.setConfig(config); }

void Conductor::setup_receiver(const std::string &aetitle, const int port) {
  m_receiver.setaetitle(aetitle);
  m_receiver.setportnumber(port);
  m_todo = OFshared_ptr<ThreadSafeQueue<DcmDataset>>(new ThreadSafeQueue<DcmDataset>);
  m_receiver.setpointer(m_todo);
  m_receiver.start();
}

void Conductor::setup_destination(const std::string &aetitle, const std::string &peer_aetitle,
                                  const std::string &hostname, const int port) {
  m_destination.set_aetitle(aetitle);
  m_destination.set_peer_aetitle(peer_aetitle);
  m_destination.set_peer_hostname(hostname);
  m_destination.set_peer_port(port);
}

void Conductor::setup_quarentine(const std::string &aetitle,const std::string &peer_aetitle,
                                 const std::string &hostname, const int port) {
  m_quarentine.set_aetitle(aetitle);
  m_quarentine.set_peer_aetitle(peer_aetitle);
  m_quarentine.set_peer_hostname(hostname);
  m_quarentine.set_peer_port(port);
}


void Conductor::process_next_dataset() {
  // wait until next dataste is available
  std::cout << "Conductor: process_next_dataset" << std::endl;
  auto dataset = m_todo->front();
  process_dataset(dataset);
  m_todo->pop();
}

void Conductor::process_dataset(DcmDataset &dataset) {
  // pipeline goes here!!!

  m_parser.setDicomDset(&dataset);
  m_parser.parse();

  OFCondition result = m_parser.allResults;
  
  
  if (result.bad()){
      OFLOG_INFO(get_logger(),"The parser has finished! Sent to quarantine!");
      auto send_result = m_quarentine.send(dataset);
  }
  else{
      OFLOG_INFO(get_logger(),"The parser has finished! Sent to destination!");
      auto send_result = m_destination.send(dataset);
  }
  //auto result = m_destination.send(dataset);
}
