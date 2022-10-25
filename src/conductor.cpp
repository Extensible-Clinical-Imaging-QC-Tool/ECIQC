#include "conductor.hpp"
#include "communication/ThreadSafeQueue.hpp"
#include <dcmtk/config/osconfig.h>
#include <exception>
#include <iostream>
#include <csignal>

volatile std::sig_atomic_t keep_running = 1;
 
void signal_handler(int signal)
{
  keep_running = signal;
}
 
void pipeline(Sender &destination, Sender &quarentine, DcmDataset &dataset) {
    auto result = destination.send(dataset);
}

void conductor(Sender &destination, Sender &quarentine, Receiver &receiver) {
  // Execute C-STORE Request with TestSCU
  OFshared_ptr<ThreadSafeQueue<DcmDataset>> received_pDset(
      new ThreadSafeQueue<DcmDataset>);
  receiver.setpointer(received_pDset);

  auto handler = std::signal(SIGINT, signal_handler);

  // Receiver listens
  receiver.start();
  while (keep_running != 0) {
    auto dataset = received_pDset->front();
    pipeline(destination, quarentine, dataset);
    received_pDset->pop();
  }
  receiver.request_stop();
  receiver.join();
}
