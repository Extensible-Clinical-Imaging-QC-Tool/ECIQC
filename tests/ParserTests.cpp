#include "parsing/Parser.hpp"
#include "metadata/MetadataEditor.hpp"
#include <iostream>
#include "catch.hpp"
//#include "testDcmMaker.cpp"

DcmTagKey pNameTagKey = DCM_PatientName;
//OFCondition res = makeTestDICOMFile();
OFString testPath = "test.dcm";
//OFString schemaPath = "../schema/useCase.json";
OFString pName;

//MetadataEditor meObjChecks{testPath};
//Parser pObj{schemaPath};

// DCM tags
DcmTagKey nameKey = DCM_PatientName;

OFString schemaPath;

//TEST_CASE("Test parser object instantiation","[NP]") {
//    std::cout << pObj.base.begin().key();
//}
//
//TEST_CASE("Test parser dset setting","[NP]") {
//    pObj.setDicomDset(testPath);
//    DcmDataset* temp = pObj.currentDataset;
//    COUT << temp;
//    temp->print(COUT);
//}

TEST_CASE("Test parsing name change", "[NP]") {
    schemaPath = "../schema/test_namechange.json";
    Parser* pObj = new Parser{schemaPath};
    pObj->setDicomDset(testPath);

    OFCondition flag;
    pObj->parse();

    CHECK(pObj->editor.equals("Robert", flag).good());

    delete pObj;
}

TEST_CASE("Test parsing UInt16 change", "[NP]") {
  schemaPath = "../schema/test_uintchange.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.equals(12, flag).good());

  delete pObj;
}

TEST_CASE("Test parsing Float64 change", "[NP]") {
  schemaPath = "../schema/test_floatchange.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.equals(2000.5, flag).good());

  delete pObj;
}

TEST_CASE("Test parsing AND with REMOVE", "[NP]") {
  schemaPath = "../schema/test_and.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.exists().bad());

  delete pObj;
}

TEST_CASE("Test parsing OR with REMOVE", "[NP]") {
  schemaPath = "../schema/test_or.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.exists().bad());

  delete pObj;
}