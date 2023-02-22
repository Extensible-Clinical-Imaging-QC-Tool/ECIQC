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
#include "metadata/MetadataEditor.hpp"



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
        "description": "check if patient is called 'James Bond' and change, if so.",
        "operations": {
            "EQUAL": {
                "otherTagString": "",
                "value": "James Bond",
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
  scu.add_file("../DICOM_Images/1-004.dcm");
  //scu.add_file("../DICOM_Images/1-1copy.dcm");
  //scu.add_file("../DICOM_Images/1-003.dcm");
  scu.run();

  // process the dataset
  conductor.process_next_dataset();
  // check that the first dataset got to the destination
  CHECK(dest_queue->size() == 1);

  //conductor.process_next_dataset();
  //CHECK(dest_queue->size() == 2);

  //conductor.process_next_dataset();
  //CHECK(dest_queue->size() == 3);

  //CHECK(quar_queue->size() == 1);

  
  /*
  auto editor = MetadataEditor("../DICOM_Images/1-1copy.dcm");

  // check whether the meta has been changed properly
  DcmDataset post_file = dest_queue->front();
  DcmDataset pre_file = *(editor.dset);

  OFString patient_name_pre;
  OFString patient_name_post;
  DcmTagKey nameTagKey = DCM_PatientName;
  pre_file.findAndGetOFString(nameTagKey,patient_name_pre);
  post_file.findAndGetOFString(nameTagKey,patient_name_post);

  std::cout << patient_name_pre << std::endl;
  std::cout <<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<std::endl;
  std::cout <<  patient_name_post << std::endl;
  std::cout << "*******************************"<<std::endl;
  CHECK(patient_name_pre == "James Bond");
  OFCondition flag;
  CHECK(editor.equals("Robert", flag).good());
  std::cout << "Check has been finished!" << std::endl;
  */
  
  
  
  
  
  /* editor_post.setDset(&post_file);
  
  OFCondition flag;
  
  //std::cout << .good() << std::endl;
  //std::cout << "##################" << std::endl;
  //std::cout << tag_string_pre << std::endl; 
  */
  // check that the second dataset got to the destination
  
  //auto dicom_file2 = dest_queue->pop();
  //auto dicom_file3 = dest_queue->pop();
}

#endif
