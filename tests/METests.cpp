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
    "testName3",
};

OFCondition res = makeTestDICOMFile();
MetadataEditor obj{"test.dcm"};

TEST_CASE("Test for CHECKING tag EXISTENCE") {

  obj.setTag(nameTagString);

  CHECK(obj.exists());
  CHECK(obj.exists(nameTagKey));
  CHECK(obj.exists(nameTagString));
  CHECK_FALSE(obj.exists(retiredTagKey));
  CHECK_FALSE(obj.exists(retiredTagString));
}

TEST_CASE("Test for DELETING DICOM elements") {

  CHECK(obj.exists(nameTagKey));
  OFCondition result = obj.deleteTag(nameTagString, OFFalse, OFFalse);
  if (result.bad()) {
    std::cout << "DL " << result.text() << "**" << std::endl;
  }
  CHECK_FALSE(obj.exists(nameTagKey));
}

TEST_CASE("Test for CREATING/MODIFYING DICOM elements") {

  std::vector<OFCondition> resultCond;
  // Creating
  CHECK_FALSE(obj.exists(nameTagKey));
  resultCond.push_back(obj.modify(newNames[0], nameTagKey, OFFalse));
  obj.dset->findAndGetOFString(nameTagKey, name);
  CHECK(name == newNames[0]);
  // Modifying
  resultCond.push_back(obj.modify(newNames[1], OFTrue));
  obj.dset->findAndGetOFString(nameTagKey, name);
  CHECK(name == newNames[1]);
  resultCond.push_back(obj.modify(newNames[2], nameTagString, OFTrue));
  obj.dset->findAndGetOFString(nameTagKey, name);
  CHECK(name == newNames[2]);

  for (auto i : resultCond) {
    if (i.bad()) {
      std::cout << i.text() << std::endl;
    }
  }
}

TEST_CASE("Test for REGEX MATCHING") {
  // IN PROGRESS
  std::string str1 = "TEsting";
  std::string str2 = "String for TEsting regex";
  OFString str_expr1 = "[a-z]+[A-Z][a-z]+[0-9]"; // testNameX
}


TEST_CASE("Test for COPYING DICOM values") {
  // IN PROGRESS

  DcmTagKey tag_a =
      /*DCM_ImagerPixelSpacing;*/ DCM_RequestedProcedureDescription;
  DcmTagKey tag_b = DCM_ImagerPixelSpacing; /*DCM_BitsStored;*/
  DcmElement *ele1, *ele2;
  obj.dset->findAndGetElement(tag_a, ele1);
  obj.dset->findAndGetElement(tag_b, ele2);
  ele1->print(std::cout);
  ele2->print(std::cout);

  int posTo = 0;
  int posFrom = 0;
  OFBool replace = OFFalse;
  OFBool copyToThis = OFTrue;

  obj.setTag(tag_b);
  OFCondition cond = obj.copyTag(tag_a, posFrom, posTo, copyToThis, replace);

  obj.dset->findAndGetElement(tag_a, ele1);
  obj.dset->findAndGetElement(tag_b, ele2);
  ele1->print(std::cout);
  ele2->print(std::cout);
  
}
