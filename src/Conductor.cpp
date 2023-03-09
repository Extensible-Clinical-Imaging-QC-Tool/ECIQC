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
#include "image/ImageEditor.hpp"
    
#include "../libs/nlohmann_json/single_include/nlohmann/json.hpp"

using json = nlohmann::json;
/// TODO - setup image editor here
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

//void Conductor setup_image_editor

void Conductor::process_next_dataset() {
  // wait until next dataset is available
  std::cout << "Conductor: process_next_dataset" << std::endl;
  //pushes dataset to the front of the queue
  auto dataset = m_todo->front();

  //actually process the dataset
  process_dataset(dataset);
  m_todo->pop();
}

void Conductor::process_dataset(DcmDataset dataset) {
  // pipeline goes here!!!
  
  // Metadata editor
  m_parser.setDicomDset(&dataset);
  m_parser.parse();

  // Image Editor
  ImageEditor dataset2edit(&dataset);
  // dataset2edit.coverText();

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
  
  // NOTE: the followings are the original version of the pipeline. There are still useful parts (e.g. ImageEditor).
  // So we just comment it out and return back afterwards.
  /*
  // 1. parse dataset
  //pull the dataset passed to "process_dataset"
  m_parser.setDicomDset(&dataset);
  //check conditions and parse
  OFCondition flag;
  m_parser.parse();

  // 2. check if dataset is valid
  //how do we check if the dataset is valid?? - parser runs check
  //but how do we identify if checks are successful
  //how do we get "actionResult" from parser

  if (m_parser.allResults.bad()) {
  //if bad send to quar:
    m_quarentine.send(dataset);
  }
  else {
  // 3. perform image edits
  //TODO - check if image edits are performed correctly
  // for now just send to image editor 
  //we need to make sure that we are running the cover text operation
  ImageEditor dataset2edit (&dataset);
  dataset2edit.coverText();

  // 4. send dataset to destination
  m_destination.send(dataset);
  
  }
  */

}


//this was giving us trouble before -
//we need to make sure that this is the same format as the unit tests??
//just ensure that we shutdown the conductor properly when done
void Conductor::shutdown_receiver() {
  m_receiver.request_stop();
  m_receiver.join();
}
