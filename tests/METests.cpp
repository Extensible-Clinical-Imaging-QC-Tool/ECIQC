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
#include "metadata/MetadataEditor.hpp"
#include "testDcmMaker.cpp"
#include "catch.hpp"
#include <regex>
#include <dcmtk/dcmpstat/dcmpstat.h>


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

OFString dblNameTagString = "(0040,9224)";
DcmTagKey dblNameTagKey = DCM_RealWorldValueIntercept;
double dblValue = 2342.03487523;
OFString uint16NameTagString = "(0028,0100)";
DcmTagKey uint16NameTagKey = DCM_BitsAllocated;
int uintValue = 16;
double checkPerturbation = 1e-3;
OFString ModelNameTagString = "(0018,702b)";
DcmTagKey ModelNameTagKey = DCM_DetectorManufacturerModelName;

OFString badTagStr = "(0008, 0080)";
DcmTagKey badKey = DCM_InstitutionName;

// Create test .dcm file
OFCondition res = makeTestDICOMFile();
MetadataEditor meObj{"test.dcm"};

DcmFileFormat test_dfile;
OFString test_filepath = "test.dcm";
OFCondition test_dset_load = test_dfile.loadFile(test_filepath.c_str());
DcmDataset* test_dset = test_dfile.getDataset();

TEST_CASE("Test for creating ME object using dataset", "[ME]") {
  MetadataEditor meObj_test{test_dset};
  meObj_test.setTag(nameTagString);
  CHECK(meObj_test.exists().good());
}

TEST_CASE("Test for CHECKING tag EXISTENCE","[ME]") {

  meObj.setTag(nameTagString);

  CHECK(meObj.exists().good());
  CHECK(meObj.exists(nameTagKey).good());
  CHECK(meObj.exists(nameTagString).good());
  CHECK_FALSE(meObj.exists(retiredTagKey).good());
  CHECK_FALSE(meObj.exists(retiredTagString).good());
}

TEST_CASE("Test for GETTING tag key", "[ME]") {
  DcmTagKey nameTagKeyTest = meObj.getTagKey();
  CHECK(nameTagKeyTest == nameTagKey);
}

TEST_CASE("Test for GETTING tag string", "[ME]") {
  OFString nameTagStringTest = meObj.getTagString();
  CHECK(nameTagStringTest == nameTagString);
}

TEST_CASE("Test for DELETING DICOM elements","[ME]") {

  CHECK(meObj.exists(nameTagKey).good());
  OFCondition result = meObj.deleteTag(nameTagString, OFFalse, OFFalse);
  if (result.bad()) {
    std::cout << "DL " << result.text() << "**" << std::endl;
  }
  CHECK_FALSE(meObj.exists(nameTagKey).good());
}

TEST_CASE("Test for CREATING/MODIFYING DICOM elements","[ME]") {

  std::vector<OFCondition> resultCond;

  //DcmElement *ele1, *ele2;
  //meObj.dset->findAndGetElement(nameTagKey, ele1);
  //meObj.dset->findAndGetElement(DCM_ImagerPixelSpacing, ele2);
  //ele1->print(std::cout);
  //ele2->print(std::cout);


  // Creating
  CHECK_FALSE(meObj.exists(nameTagKey).good());
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
//  std::string str1 = "TEsting";
//  std::string str2 = "String for TEsting regex";
  OFString str_expr1 = "[A-Z][a-z]+\\s[A-Z][a-z]+"; // Sidri Able
  OFString str_expr2 = "[a-z]+[A-Z]*[a-z]+[0-9]*"; // Does not match - would match testName[number]

  //std::cout << "here";
  OFString writtenTag = "DCM_PatientName";

  OFCondition flag;
  CHECK(meObj.match(str_expr1, flag).good());
  CHECK_FALSE(meObj.match(str_expr2, flag).good());

  CHECK(meObj.match(nameTagString,str_expr1, flag).good());
  CHECK_FALSE(meObj.match(nameTagString, str_expr2, flag).good());

  CHECK(meObj.match(nameTagKey,str_expr1, flag).good());
  CHECK_FALSE(meObj.match(nameTagKey, str_expr2, flag).good());
//  std::cout << "Match output " << meObj.match(str_expr1, flag).text();
}

TEST_CASE("Test for FAILED MATCH", "[ME]") {
  OFString str_expr1 = "[A-Z][a-z]+\\s[A-Z][a-z]+"; // Sidri Able
  OFCondition flag;

  meObj.setTag(badKey);
  CHECK_FALSE(meObj.match(str_expr1, flag).good());

  meObj.setTag(nameTagString);
  CHECK_FALSE(meObj.match(badTagStr, str_expr1, flag).good());
  CHECK_FALSE(meObj.match(badKey, str_expr1, flag).good());
}

TEST_CASE("Test for CHECKING tag EQUALITY","[ME]") {
    OFCondition flag;
    meObj.setTag(nameTagString);
    OFString badString = "Not a match";

    CHECK(meObj.equals(newNames[2], flag).good());
    CHECK(meObj.equals(nameTagKey, newNames[2], flag).good());
    CHECK(meObj.equals(nameTagString, newNames[2], flag).good());
    CHECK_FALSE(meObj.equals(badString, flag).good());
    CHECK_FALSE(meObj.equals(nameTagKey, badString, flag).good());
    CHECK_FALSE(meObj.equals(nameTagString, badString, flag).good());

    meObj.setTag(dblNameTagString);
    CHECK(meObj.equals(dblValue, flag).good());
    CHECK_FALSE(meObj.equals(dblValue-checkPerturbation, flag).good());

    CHECK(meObj.equals(dblNameTagString, dblValue, flag).good());
    CHECK_FALSE(meObj.equals(dblNameTagString, dblValue-checkPerturbation, flag).good());
    CHECK(meObj.equals(uint16NameTagString, uintValue, flag).good());
    CHECK_FALSE(meObj.equals(uint16NameTagString, uintValue+checkPerturbation, flag).good());

    CHECK(meObj.equals(dblNameTagKey, dblValue, flag).good());
    CHECK_FALSE(meObj.equals(dblNameTagKey, dblValue-checkPerturbation, flag).good());
    CHECK(meObj.equals(uint16NameTagKey, uintValue, flag).good());
    CHECK_FALSE(meObj.equals(uint16NameTagKey, uintValue+checkPerturbation, flag).good());
}

TEST_CASE("Test for TAG EQUALS function failures","[ME]") {
  OFCondition flag;
  meObj.setTag(badKey);
  OFString badString = "Not a match";

  CHECK_FALSE(meObj.equals(badString, flag).good());
  CHECK_FALSE(meObj.equals(DCM_InstitutionName, badString, flag).good());
  CHECK_FALSE(meObj.equals(badTagStr, badString, flag).good());

  CHECK_FALSE(meObj.equals(dblValue-checkPerturbation, flag).good());
  CHECK_FALSE(meObj.equals(badTagStr, dblValue-checkPerturbation, flag).good());
  CHECK_FALSE(meObj.equals(badTagStr, uintValue+checkPerturbation, flag).good());
  CHECK_FALSE(meObj.equals(badKey, dblValue-checkPerturbation, flag).good());
  CHECK_FALSE(meObj.equals(badKey, uintValue+checkPerturbation, flag).good());
}

TEST_CASE("Test for CHECKING tag GREATER THAN and LESS THAN","[ME]") {
    OFCondition flag;
    meObj.setTag(uint16NameTagKey); // Changed to set tag by key to increase test coverage

//  'This' tag
    CHECK(meObj.greaterOrLessThan(uintValue-checkPerturbation, OFTrue, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(uintValue+checkPerturbation, OFTrue, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(uintValue, OFTrue, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(uintValue, OFFalse, flag).good());

//  String tags
    CHECK(meObj.greaterOrLessThan(dblNameTagString, dblValue-checkPerturbation, OFTrue, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(dblNameTagString, dblValue+checkPerturbation, OFTrue, flag).good());
    CHECK(meObj.greaterOrLessThan(dblNameTagString, dblValue+checkPerturbation, OFFalse, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(dblNameTagString, dblValue-checkPerturbation, OFFalse, flag).good());
    CHECK(meObj.greaterOrLessThan(uint16NameTagString, uintValue-checkPerturbation, OFTrue, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(uint16NameTagString, uintValue+checkPerturbation, OFTrue, flag).good());
    CHECK(meObj.greaterOrLessThan(uint16NameTagString, uintValue+checkPerturbation, OFFalse, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(uint16NameTagString, uintValue-checkPerturbation, OFFalse, flag).good());
//  Strict inequalities
    CHECK_FALSE(meObj.greaterOrLessThan(dblNameTagString, dblValue, OFTrue, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(dblNameTagString, dblValue, OFFalse, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(uint16NameTagString, uintValue, OFTrue, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(uint16NameTagString, uintValue, OFFalse, flag).good());


//  Dicom key tags
    CHECK(meObj.greaterOrLessThan(dblNameTagKey, dblValue-checkPerturbation, OFTrue, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(dblNameTagKey, dblValue+checkPerturbation, OFTrue, flag).good());
    CHECK(meObj.greaterOrLessThan(dblNameTagKey, dblValue+checkPerturbation, OFFalse, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(dblNameTagKey, dblValue-checkPerturbation, OFFalse, flag).good());
    CHECK(meObj.greaterOrLessThan(uint16NameTagKey, uintValue-checkPerturbation, OFTrue, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(uint16NameTagKey, uintValue+checkPerturbation, OFTrue, flag).good());
    CHECK(meObj.greaterOrLessThan(uint16NameTagKey, uintValue+checkPerturbation, OFFalse, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(uint16NameTagKey, uintValue-checkPerturbation, OFFalse, flag).good());
//  Strict inequalities
    CHECK_FALSE(meObj.greaterOrLessThan(dblNameTagKey, dblValue, OFTrue, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(dblNameTagKey, dblValue, OFFalse, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(uint16NameTagKey, uintValue, OFTrue, flag).good());
    CHECK_FALSE(meObj.greaterOrLessThan(uint16NameTagKey, uintValue, OFFalse, flag).good());
}

TEST_CASE("Test for CHECKING tag APPEND and PREPEND","[ME]"){
    meObj.setTag(DCM_PatientName);
    OFCondition flag;
    OFString test1 = "test1";
    OFString test2 = "test2";
    OFString test3 = "test3";

//    Appends:
    meObj.append(test1, flag);
    CHECK(meObj.equals(newNames[2] + test1, flag).good());

    meObj.append(test2, nameTagString, flag);
    CHECK(meObj.equals(newNames[2] + test1 + test2, flag).good());

    meObj.append(test3, nameTagKey, flag);
    CHECK(meObj.equals(newNames[2] + test1 + test2 + test3, flag).good());

//    Prepends:
    meObj.prepend(test1, flag);
    CHECK(meObj.equals(test1 + newNames[2] + test1 + test2 + test3, flag).good());

    meObj.prepend(test2, nameTagString, flag);
    CHECK(meObj.equals(test2 + test1 + newNames[2] + test1 + test2 + test3, flag).good());

    meObj.prepend(test3, nameTagKey, flag);
    CHECK(meObj.equals(test3 + test2 + test1 + newNames[2] + test1 + test2 + test3, flag).good());
}

TEST_CASE("Test for COPYING DICOM values","[ME]") {
    // IN PROGRESS

    DcmTagKey NameTag =
            /*DCM_ImagerPixelSpacing; DCM_RequestedProcedureDescription*/ DCM_PatientName;
    DcmTagKey PixelTag = DCM_ImagerPixelSpacing; /*DCM_BitsStored;*/
    DcmTagKey FieldTag = DCM_FieldOfViewOrigin;
    DcmTagKey BitsTag = DCM_BitsAllocated;
    DcmTagKey HighBitTag = DCM_HighBit;
    DcmTagKey InterceptTag = DCM_RealWorldValueIntercept;
    DcmTagKey SlopeTag = DCM_RealWorldValueSlope;

    OFString str_NameTag = "(0010,0010)";
    OFString str_PixelTag = "(0018,2264)";
    OFString str_FieldTag = "(0018,7030)";
    OFString str_BitsTag = "(0028,0100)";
    OFString str_HighBitTag = "(0028,0102)";
    OFString str_InterceptTag = "(0040,9224)";
    OFString str_SlopeTag = "(0040,9225)";

    OFString str_name, str_pixel, str_field;
    Uint16 int_bits, int_highbit;
    Float64 dbl_intercept, dbl_slope;

    const unsigned long posTo = 0;
    const unsigned long posFrom = 0;
    OFBool replace = OFFalse;
    OFBool copyToThis = OFTrue;

//  Test you can't copy from retiredTagKey
    meObj.setTag(PixelTag);
    OFCondition cond = meObj.copy(retiredTagKey, posFrom, posTo, copyToThis, replace);
    CHECK(cond.bad()); // Cannot copy from retiredTagKey - check copy fails

//  Test you can't copy from retiredTagString
    meObj.setTag(PixelTag);
    cond = meObj.copy(retiredTagString, posFrom, posTo, copyToThis, replace);
    CHECK(cond.bad()); // Cannot copy from retiredTagKey - check copy fails

//  Testing copy UInt16, copyToThis == OFFalse, replace == OFTrue, using OFString tag
    meObj.setTag(BitsTag);
    meObj.dset->findAndGetUint16(BitsTag, int_bits);
    meObj.dset->findAndGetUint16(HighBitTag, int_highbit);
    CHECK_FALSE(int_bits==int_highbit); // elements are different before copy

    cond = meObj.copy(str_HighBitTag, posFrom, posTo, !copyToThis, !replace);
    CHECK(cond.good()); // copy without error

    meObj.dset->findAndGetUint16(BitsTag, int_bits);
    meObj.dset->findAndGetUint16(HighBitTag, int_highbit);
    CHECK(int_bits==int_highbit); // elements match after copy

//  Testing copy Float64, copyToThis == OFTrue, replace == OFFalse, using DcmTagKey
    meObj.setTag(InterceptTag);
    meObj.dset->findAndGetFloat64(InterceptTag, dbl_intercept);
    meObj.dset->findAndGetFloat64(SlopeTag, dbl_slope);
    CHECK_FALSE(dbl_intercept==dbl_slope); // elements are different before copy

    cond = meObj.copy(SlopeTag, posFrom, posTo, copyToThis, replace);
    CHECK(cond.good()); // copy without error

    meObj.dset->findAndGetFloat64(InterceptTag, dbl_intercept);
    meObj.dset->findAndGetFloat64(SlopeTag, dbl_slope);
    CHECK(dbl_intercept==dbl_slope); // elements match after copy

//  Testing copy StringArray, copyToThis == OFTrue, replace == OFFalse, using DcmTagKey
    meObj.setTag(PixelTag);
    meObj.dset->findAndGetOFStringArray(PixelTag, str_pixel);
    meObj.dset->findAndGetOFStringArray(FieldTag, str_field);
    CHECK_FALSE(str_pixel==str_field); // elements are different before copy

    cond = meObj.copy(FieldTag, posFrom, posTo, copyToThis, replace);
    CHECK(cond.good()); // copy without error
    cond = meObj.copy(FieldTag, posFrom+1, posTo+1, copyToThis, replace);
    CHECK(cond.good()); // copy without error

    meObj.dset->findAndGetOFStringArray(PixelTag, str_pixel);
    meObj.dset->findAndGetOFStringArray(FieldTag, str_field);
    CHECK_FALSE(str_pixel==str_field); // elements shouldn't match after non-replace

    std::size_t posn = str_pixel.find(str_field);
    CHECK(posn==0); //str_field is found at position 0 in str_pixel

}

TEST_CASE("Test for OVERWRITE of values using regex", "[ME]"){
    meObj.setTag(ModelNameTagKey);
    OFCondition flag;
    DcmTagKey DOB_TagKey;

    OFString reg_expr = "([0-9]{4})([0-9]{4})";
    OFString replace_with = "$010101";
    meObj.overwrite(DOB_TagKey, reg_expr, replace_with);

    // Check we can replace part of "this" tag
    DcmTagKey ModelTag = DCM_DetectorManufacturerModelName;
    OFString str_model;
    OFString str_expr_1 = "[0-9]{4}";
    OFString replaceString_1 = "0101";
    meObj.dset->findAndGetOFString(ModelTag, str_model);
    CHECK(str_model == "PIXIUM4600");
    flag = meObj.overwrite(str_expr_1, replaceString_1);
    CHECK(flag.good());
    meObj.dset->findAndGetOFString(ModelTag, str_model);
    CHECK(str_model == "PIXIUM"+replaceString_1);

    // Check we can replace part of another tag given a string
    OFString replaceString_2 = "zxxxz";
    flag = meObj.overwrite(ModelNameTagString, str_expr_1, replaceString_2);
    CHECK(flag.good());
    meObj.dset->findAndGetOFString(ModelTag, str_model);
    CHECK(str_model == "PIXIUM"+replaceString_2);

    // Check we can replace part of another tag given a key
    OFString str_expr_2 = "([A-Z]+)([a-z]+)";
    OFString replaceString_3 = "$01$01$02$02";
    flag = meObj.overwrite(ModelNameTagKey, str_expr_2, replaceString_3);
    CHECK(flag.good());
    meObj.dset->findAndGetOFString(ModelTag, str_model);
    CHECK(str_model == "PIXIUMPIXIUM"+replaceString_2+replaceString_2);
}

TEST_CASE("Test for FAILS of OVERWRITE of values using regex", "[ME]"){
    meObj.setTag(retiredTagString);
    OFCondition flag;

    // Check we can't replace part of "this" tag
    OFString str_expr_1 = "[0-9]{4}";
    OFString replaceString_1 = "0101";
    flag = meObj.overwrite(str_expr_1, replaceString_1);
    CHECK_FALSE(flag.good());

    // Check we can replace part of another tag given a string
    OFString replaceString_2 = "zxxxz";
    flag = meObj.overwrite(retiredTagString, str_expr_1, replaceString_2);
    CHECK_FALSE(flag.good());

    // Check we can replace part of another tag given a key
    OFString str_expr_2 = "([A-Z]+)([a-z]+)";
    OFString replaceString_3 = "$01$01$02$02";
    flag = meObj.overwrite(retiredTagKey, str_expr_2, replaceString_3);
    CHECK_FALSE(flag.good());
}

/* Code for running selective tests " {path to unit_tests} [ME] "*/