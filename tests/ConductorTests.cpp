#include "catch.hpp"
#include <exception>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <type_traits>

#include "TestStorageSCU.hpp"
#include "Conductor.hpp"
#include <dcmtk/config/osconfig.h>

#ifdef WITH_THREADS
TEST_CASE(
    "Test Conductor in sending C-STORE Request for a list of Passed Datasets",
    "[CT]") {
  const auto *config_template = R"( 
  {
    "receiver": {
      "aetitle": "TestQCTool",
      "port": 11112
    },
    "destination": {
      "aetitle": "TestDestination",
      "hostname": "localhost",
      "port": 11113
    },
    "quarentine": {
      "aetitle": "TestQuarentine",
      "hostname": "localhost",
      "port": 11114
    },
    "metadata": {
      "put_tag_here": {
        "tagName": "PatientName",
        "vr": "CS",
        "description": "check if patient is called 'bob' and change, if so.",
        "operations": {
            "EQUAL": {
                "otherTagString": "",
                "value": "John Doe",
                "IF_TRUE": {
                    "OVERWRITE": {
                        "tag": "",
                        "replaceString": "Robert"
                    }
                }
            }
        }
      }
    }
  })";

  // replace tag cause we can't use it in the R string
  auto config = std::regex_replace(config_template, std::regex("put_tag_here"),
                                   "(0010,0010)"); // replace 'def' -> 'klm'

  // create a destination receiver
  OFshared_ptr<ThreadSafeQueue<DcmDataset>> dest_queue(
      new ThreadSafeQueue<DcmDataset>);
  Receiver destination(11113, "Destination");
  destination.setpointer(dest_queue);
  destination.start();

  // create a quarentine receiver
  OFshared_ptr<ThreadSafeQueue<DcmDataset>> quar_queue(
      new ThreadSafeQueue<DcmDataset>);
  Receiver quarentine(11114, "Quarentine");
  quarentine.setpointer(quar_queue);
  quarentine.start();

  std::stringstream config_ss(config);
  Conductor conductor(config_ss);

  // Configure SCUs.
  TestStorageSCU scu;
  scu.set_peer("TestQCTool", 11112);
  scu.initialise();
  scu.add_file("../DICOM_Images/1-1copy.dcm");
  scu.add_file("../DICOM_Images/test2.dcm");
  scu.run();

  // process the dataset
  conductor.process_next_dataset();

  // check that the first dataset got to the destination
  CHECK(dest_queue->size() == 1);

  conductor.process_next_dataset();

  // check that the second dataset got to the destination
  CHECK(dest_queue->size() == 2);
}

#endif
