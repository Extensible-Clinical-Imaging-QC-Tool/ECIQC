#include "catch.hpp"
#include "communication/ThreadSafeQueue.hpp"
#include <iostream>
#include <limits>
#include <type_traits>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcistrmb.h>

#ifdef WITH_THREADS

#include <dcmtk/dcmnet/dstorscu.h>
#include <dcmtk/dcmnet/scu.h>
#include <dcmtk/dcmpstat/dcmpstat.h>

#include "misc/Exception.hpp"
#include "misc/MyLibrary.hpp"

#include "TestStorageSCU.hpp"
#include "communication/Receiver.hpp"
#include "logging.hpp"

using namespace cpp_template;


// This tests handling of C-ECHO Association and also the logging
TEST_CASE("Test C-ECHO Association and the logging", "[RTN]") {
  set_root_logging("../TestReceiver_root.log", true);
  set_logging("../TestReceiver.log", true);
  const int port = 11112;
  const std::string title = "TestSCP";
  Receiver pool(port, title);
  pool.start();

  OFVector<TestStorageSCU> scus(2);
  for (auto &scu : scus) {
    scu.set_peer(title, port);
    scu.initialise();
    scu.run();
  }

  // Check the association.
  for (auto &scu : scus) {
    CHECK(scu.get_neg_association_result().good());
    CHECK(scu.get_echo_result().good());
    CHECK(scu.get_release_association_result().good());
  }
  pool.request_stop();
  pool.join();
}

// This tests handling of C-ECHO Association
TEST_CASE("Test C-ECHO Association", "[RT]") {
  const int port = 11112;
  const std::string title = "TestSCP";
  Receiver pool(port, title);
  pool.start();

  OFVector<TestStorageSCU> scus(2);
  for (auto &scu : scus) {
    scu.set_peer(title, port);
    scu.initialise();
    scu.run();
  }

  // Check the association.
  for (auto &scu : scus) {
    CHECK(scu.get_neg_association_result().good());
    CHECK(scu.get_echo_result().good());
    CHECK(scu.get_release_association_result().good());
  }
  pool.request_stop();
  pool.join();
}

// This tests handling of C-STORE Association
TEST_CASE("Test C-STORE Association", "[RT1]") {

  const int port = 11112;
  const std::string title = "TestSCP";
  OFshared_ptr<ThreadSafeQueue<DcmDataset>> pt(new ThreadSafeQueue<DcmDataset>);
  Receiver pool(port, title);
  pool.setpointer(pt);
  pool.start();

  // Configure SCUs.
  OFVector<TestStorageSCU> scus(2);
  for (auto &scu : scus) {
    scu.set_peer(title, port);
    scu.initialise();
    scu.add_file("../DICOM_Images/1-003.dcm");
    scu.add_file("../DICOM_Images/1-004.dcm");
    scu.run();
  }

  pool.request_stop();
  pool.join();


  // Check the association.
  for (auto &scu : scus) {
    CHECK(scu.get_neg_association_result().good());
    CHECK(scu.get_echo_result().good());
    CHECK(scu.get_release_association_result().good());
    for (auto &result : scu.get_results()) {
      CHECK(result.good());
    }
  }

  // check we got the datasets
  CHECK(pt->size() == 4);
  pt.reset();

  pool.request_stop();
  pool.join();

}

// This tests basic configuration of the Receiver.
TEST_CASE("Test Receiver Configuration", "[RT]") {

  Receiver scppool(104, "ATestName");

  CHECK(scppool.getConfig().getAETitle() == "ATestName");
  CHECK(scppool.getConfig().getPort() == 104);

  OFList<OFString> hostiplist;
  hostiplist.push_back("An IP address");

  OFList<OFString> peeraelist;
  peeraelist.push_back("A test name");

  scppool.setacceptableIPs(hostiplist);
  scppool.setcallingAETitles(peeraelist);

  CHECK(scppool.getacceptableIPs().size() == 1);
  CHECK(scppool.getcallingAETitles().size() == 1);
}

// This tests datasource check at hostname/IP level
TEST_CASE("Test hostname/IP check - Accept") {

  // Check that a specified hostname is accepted.
  Receiver pool(11112, "TestSCP");

  OFList<OFString> hostiplist;
  hostiplist.push_back("localhost");

  pool.setacceptableIPs(hostiplist);
  // Start listening
  pool.start();

  TestStorageSCU scu("PoolTestSCU", "TestSCP", 11112);
  scu.initialise();
  scu.run();
  CHECK(scu.get_neg_association_result().good());
  CHECK(scu.get_release_association_result().good());
  CHECK(scu.get_echo_result().good());

  pool.request_stop();
  pool.join();
}

TEST_CASE("Test hostname/IP check - Reject", "[RT]") {
  // Check that a specified hostname is accepted.
  Receiver pool(11112, "TestSCP");

  OFList<OFString> hostiplist;
  hostiplist.push_back("aRandomIP");

  pool.setacceptableIPs(hostiplist);
  // Start listening
  pool.start();

  TestStorageSCU scu("PoolTestSCU", "TestSCP", 11112);
  scu.initialise();
  scu.run();
  CHECK(scu.get_neg_association_result().bad());
  pool.request_stop();
  pool.join();
}

// This tests datasource check at hostname/IP level
TEST_CASE("Test called AE Title check", "[RT]") {
  const std::string scu_title = "PoolTestSCU";
  const std::string recv_title = "TestSCP";
  const int port = 11112;

  // Check that a specified AE title is accepted.
  Receiver pool(port, recv_title);

  OFList<OFString> aetitles;
  aetitles.push_back(scu_title.c_str());
  pool.setcallingAETitles(aetitles);
  // Start listening
  pool.start();

  // Configure SCU and initialize
  TestStorageSCU scu(scu_title, recv_title, port);
  scu.initialise();
  scu.run();

  // Request shutdown.
  pool.request_stop();
  pool.join();

  CHECK(scu.get_neg_association_result().good());
  CHECK(scu.get_release_association_result().good());
  CHECK(scu.get_echo_result().good());

  // Check that a non-specified hostname is rejected.
  Receiver poolrej(port, recv_title);

  OFList<OFString> aelistrej;
  aelistrej.push_back("A Title");
  poolrej.setcallingAETitles(aelistrej);
  poolrej.start();

  scu.initialise();
  scu.run();

  // Request shutdown.
  poolrej.request_stop();
  poolrej.join();

  CHECK(scu.get_neg_association_result().bad());

  
}

#endif
