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
#include "catch.hpp"

#include <limits>
#include <type_traits>

#include "../src/Exception.hpp"
//#include "../src/MyLibrary.hpp"
#include <iostream>
#include <dcmtk/dcmpstat/dcmpstat.h>
#include "../src/MetadataEditor.cpp"
#include "../src/MetadataEditor.hpp"

using namespace cpp_template;

// This tests the output of the `get_nth_prime` function
TEST_CASE("correct primes are returned", "[primes]") {

  DcmFileFormat image;
  OFCondition status = image.loadFile("../DICOM_Images/1-1copy.dcm");
  
if (status.good())
{
  OFString patientName;
  if (image.getDataset()->findAndGetOFString(DCM_PatientName, patientName).good())
  {
    std::cout << "Patient's Name: " << patientName << std::endl;
  } else
    std::cerr << "Error: cannot access Patient's Name!" << std::endl;
} else
  std::cerr << "Error: cannot read DICOM file (" << status.text() << ")" << std::endl;

}

TEST_CASE("Testing the metadata editing class") {
  std::cout << "## Metadata editing class ## \n";
  int hh = 3;
  MetadataEditor obj{hh};

  obj.print();
}


