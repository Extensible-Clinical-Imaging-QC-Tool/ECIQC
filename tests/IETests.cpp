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
#include "image/ImageEditor.hpp"
#include "catch.hpp"
#include <regex>
#include <memory>
//
Uint16* pixelData;
//
TEST_CASE("Test for reading DICOM images and converting to MAT object for openCV","[DR]") {
    OFString filePath = OFString("../DICOM_Images/GEMS_IMG/2021_NOV/18/_C103258/LBIAGEG2");
   // OFString filePath = OFString("../DICOM_Images/GEMS_IMG/2019_MAR/20/_C130056/J3KD2EG2");
    ImageEditor dicomTest {filePath};
    std::cout << dicomTest.getSlices()[0].size() << std::endl;
    std::cout << dicomTest.getSlices()[0].channels() << std::endl;
//    cv::namedWindow( "Final window", cv::WINDOW_AUTOSIZE );// Create a window for display.
//    cv::imshow( "Final window", dicomTest->getSlices()[0]);
//    cv::waitKey(0);
    // Display the saved image to the dataset
    dicomTest.displayFirstFrame();
}
//




/* Code for running selective tests " {path to unit_tests} [ME] "*/
