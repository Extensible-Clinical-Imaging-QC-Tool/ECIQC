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
#include "../src/MetadataEditor.cpp"
#include "../src/MetadataEditor.hpp"
#include "catch.hpp"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <iostream>
#include <regex>
#include <string>

OFString name;
OFCondition resultCond;
OFBool resultBool;

OFString path = "../DICOM_Images/1-1copy.dcm";
OFString nameTagString = "(0010,0010)";
DcmTagKey nameTagKey = DCM_PatientName;
OFString retiredTagString = "(0040,0330)";
DcmTagKey retiredTagKey = DCM_RETIRED_ReferencedProcedureStepSequence;
OFString newName = "testName";

MetadataEditor obj{path};

// Metadata editing tests

TEST_CASE("Test for OVERWRITING DICOM element values") {

  obj.setTag(nameTagString);
  obj.dset->findAndGetOFString(DCM_PatientName, name);
  CHECK(name != newName);

  resultCond = obj.modify(newName, OFTrue);
  std::string s(newName.c_str());

  if (resultCond.bad()) {
    std::cout << "OW " << resultCond.text() << "**" << std::endl;
  }

  // test other input

  obj.dset->findAndGetOFString(nameTagKey, name);

  CHECK(resultCond.good());
  CHECK(name == newName);
}

TEST_CASE("Test for CHECKING tag EXISTENCE") {
  OFBool resultBoola, resultBoolb, resultBoolc;
  resultBoola = obj.exists();
  resultBoolb = obj.exists(nameTagKey);
  resultBoolc = obj.exists(nameTagString);
  OFBool res1 = resultBoola & resultBoolb & resultBoolc;
  CHECK(res1);
  if(!res1) {
    std::cout << resultBoola << resultBoolb << resultBoolc << std::endl;
  }
  
  resultBoola = obj.exists(retiredTagKey);
  resultBoolb = obj.exists(retiredTagString);
  OFBool res2 = resultBoola & resultBoolb;
  CHECK_FALSE(res2);
  if(!res2) {
    std::cout << resultBoola << resultBoolb << std::endl;
  }
}

TEST_CASE("Test for DELETING DICOM elements") {

  CHECK(obj.exists(DCM_PatientName));
  resultCond = obj.deleteTag(nameTagString, OFFalse, OFFalse);
  if (resultCond.bad()) {
    std::cout << "DL " << resultCond.text() << "**" << std::endl;
  }
  CHECK_FALSE(obj.exists(nameTagKey));
}

TEST_CASE("Test for CREATING/INSERTING DICOM elements") {

  CHECK_FALSE(obj.exists(nameTagKey));
  resultCond = obj.modify(newName, OFFalse, nameTagString);
  if (resultCond.bad()) {
    std::cout << "CI " << resultCond.text() << "**" << std::endl;
  }
  CHECK(obj.exists(nameTagKey));
}

TEST_CASE("Test for REGEX MATCHING") {
  std::string str = "String for TEsting regex";
  std::string str2 = "TEsting";
  std::regex str_expr("([A-Z]+)([a-z]*)");
  std::smatch sm;

  // bool h = std::regex_search(str,sm,str_expr,
  // std::regex_constants::match_default); bool i =
  // std::regex_match(str2,sm,str_expr, std::regex_constants::match_default);
  // std::cout << "Match result = " <<  h << i;
}

TEST_CASE("Test for COPYING DICOM values") {
  // Read modifyOrInsertFromFile()


}
