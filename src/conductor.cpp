#include "conductor.hpp"
#include "communication/ThreadSafeQueue.hpp"
#include "memory"
#include <csignal>
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/ofstd/ofmem.h>
#include <exception>
#include <iostream>

Conductor::~Conductor() {
  m_receiver.request_stop();
  m_receiver.join();
}

void Conductor::setup_receiver(const std::string &aetitle, const int port) {
  m_receiver.setaetitle(aetitle);
  m_receiver.setportnumber(port);
  m_todo = std::make_shared<ThreadSafeQueue<DcmDataset>>();
  m_receiver.setpointer(m_todo);
}

void Conductor::setup_destination(const std::string &aetitle,
                                  const std::string &hostname, const int port) {
  m_destination.set_aetitle("Destination Sender");
  m_destination.set_peer_aetitle(aetitle);
  m_destination.set_peer_hostname(hostname);
  m_destination.set_peer_port(port);
}

void Conductor::setup_quarentine(const std::string &aetitle,
                                 const std::string &hostname, const int port) {
  m_quarentine.set_aetitle("Quarentine Sender");
  m_quarentine.set_peer_aetitle(aetitle);
  m_quarentine.set_peer_hostname(hostname);
  m_quarentine.set_peer_port(port);
}

void Conductor::finalise_initialisation() { m_receiver.start(); }

void Conductor::process_next_dataset() {
  // wait until next dataste is available
  auto dataset = m_todo->front();
  process_dataset(dataset);
  m_todo->pop();
}

void Conductor::process_dataset(DcmDataset &dataset) {
  // pipeline goes here!!!
  auto result = m_destination.send(dataset);
}
