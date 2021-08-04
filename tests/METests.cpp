/*
Copyright (c) 2018, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of the Oxford RSE C++ Template project.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include "MetadataEditor.hpp"
#include "testDcmMaker.cpp"
#include "catch.hpp"
#include <regex>


OFString name;
OFString nameTagString = "(0010,0010)";
DcmTagKey nameTagKey = DCM_PatientName;
OFString retiredTagString = "(0040,0330)";
DcmTagKey retiredTagKey = DCM_RETIRED_ReferencedProcedureStepSequence;
std::vector<OFString> newNames = {
    "testName1",
    "testName2",
    "Sidri Able"//"testName3",
};

// Create test .dcm file
OFCondition res = makeTestDICOMFile();
MetadataEditor meObj{"test.dcm"};

TEST_CASE("Test for CHECKING tag EXISTENCE","[ME]") {

  meObj.setTag(nameTagString);

  CHECK(meObj.exists());
  CHECK(meObj.exists(nameTagKey));
  CHECK(meObj.exists(nameTagString));
  CHECK_FALSE(meObj.exists(retiredTagKey));
  CHECK_FALSE(meObj.exists(retiredTagString));
}

TEST_CASE("Test for DELETING DICOM elements","[ME]") {

  CHECK(meObj.exists(nameTagKey));
  OFCondition result = meObj.deleteTag(nameTagString, OFFalse, OFFalse);
  if (result.bad()) {
    std::cout << "DL " << result.text() << "**" << std::endl;
  }
  CHECK_FALSE(meObj.exists(nameTagKey));
}

TEST_CASE("Test for CREATING/MODIFYING DICOM elements","[ME]") {

  std::vector<OFCondition> resultCond;

  //DcmElement *ele1, *ele2;
  //meObj.dset->findAndGetElement(nameTagKey, ele1);
  //meObj.dset->findAndGetElement(DCM_ImagerPixelSpacing, ele2);
  //ele1->print(std::cout);
  //ele2->print(std::cout);


  // Creating
  CHECK_FALSE(meObj.exists(nameTagKey));
  resultCond.push_back(meObj.modify(newNames[0], nameTagKey, OFFalse));
  meObj.dset->findAndGetOFString(nameTagKey, name);
  CHECK(name == newNames[0]);
  // Modifying
  resultCond.push_back(meObj.modify(newNames[1], OFTrue));
  meObj.dset->findAndGetOFString(nameTagKey, name);
  CHECK(name == newNames[1]);
  resultCond.push_back(meObj.modify(newNames[2], nameTagString, OFTrue));
  meObj.dset->findAndGetOFString(nameTagKey, name);
  CHECK(name == newNames[2]);

  for (auto i : resultCond) {
    if (i.bad()) {
      std::cout << i.text() << std::endl;
    } 
  }


  //meObj.dset->findAndGetElement(tag_a, ele1);
  //meObj.dset->findAndGetElement(tag_b, ele2);
  //ele1->print(std::cout);
  //ele2->print(std::cout);
}

TEST_CASE("Test for REGEX MATCHING","[ME]") {
  // IN PROGRESS :: Implementation in MetadataEditor.cpp
  std::string str1 = "TEsting";
  std::string str2 = "String for TEsting regex";
  OFString str_expr1 = "[a-z]+[A-Z][a-z]+[0-9]"; // testNameX

  //std::cout << "here";
  OFString writtenTag = "DCM_PatientName";
  //const DcmDataDictionary& globalD = dcmDataDict.rdlock();
  //const DcmDictEntry *ent = globalD.findEntry(writtenTag.c_str());
  //std::cout << (ent->getKey());
  //dcmDataDict.rdlock();
}


TEST_CASE("Test for COPYING DICOM values","[ME]") {
  // IN PROGRESS

  DcmTagKey tag_a =
      /*DCM_ImagerPixelSpacing; DCM_RequestedProcedureDescription*/ DCM_PatientName;
  DcmTagKey tag_b = DCM_ImagerPixelSpacing; /*DCM_BitsStored;*/
  DcmElement *ele1, *ele2;
  meObj.dset->findAndGetElement(tag_a, ele1);
  meObj.dset->findAndGetElement(tag_b, ele2);
  ele1->print(std::cout);
  ele2->print(std::cout);

  int posTo = 0;
  int posFrom = 0;
  OFBool replace = OFFalse;
  OFBool copyToThis = OFTrue;

  meObj.setTag(tag_b);
  OFCondition cond = meObj.copy(retiredTagKey, posFrom, posTo, copyToThis, replace);
  std::cout<< cond.text() << std::endl;
  meObj.dset->findAndGetElement(tag_a, ele1);
  meObj.dset->findAndGetElement(tag_b, ele2);
  ele1->print(std::cout);
  ele2->print(std::cout);

  //dset->print(COUT);
  
}



/* Code for running selective tests " {path to unit_tests} [ME] "*/