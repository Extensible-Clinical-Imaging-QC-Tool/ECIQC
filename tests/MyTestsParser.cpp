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
#include <nlohmann/json.hpp>

#include "../src/Exception.hpp"
#include "../src/MyLibrary.hpp"

#include "../src/receiver.hpp"



#include "../src/parser.cpp"
#include "../src/Parser.hpp"



using namespace cpp_template;
// Metadata editing tests

std::ifstream file("../src/schema.json");// test case for input json > parser catch done

string test_configuration;

Parser obj;


TEST_CASE("Test the initial settings of the Parser class") {


  ostringstream ss;
  ss << file.rdbuf(); // reading data
  test_configuration = ss.str();

  
  string Tag = "(10,0010)";
  string VR = "CS";
  auto input_tag = obj.initial_settings(std::stringstream(test_configuration), Tag, VR);
  auto expect = false;
  CHECK(expect == input_tag);
}

TEST_CASE("Test the parser of the config file") {


    string cfg_error;
    string ss_error = "{'(0010,0010)' : 1 }";
    cfg_error = ss_error;

    auto input = obj.file_opening(std::stringstream(cfg_error));
		auto expected = false;
		CHECK(expected == input);

    string cfg;
    string ss_test = "{ \"(0010,0010)\":1 }";
    cfg = ss_test;

    auto input_test = obj.file_opening(std::stringstream(cfg));
		auto expected_test = true; 
    CHECK(expected_test == input_test);


}

TEST_CASE("Test right inputs for the Parser class") {

  string output = obj.editing(std::stringstream(test_configuration));
  string expected = "modifying tag itself";
  CHECK(expected != output);

}


