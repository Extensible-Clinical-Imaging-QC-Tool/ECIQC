#include "parsing/Parser.hpp"
#include "metadata/MetadataEditor.hpp"
#include <iostream>
#include "catch.hpp"
#include "logging.hpp"
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


TEST_CASE("Test parsing name change, also add the logging", "[NPN]") {
    set_root_logging("../TestParser_root.log", true);
    set_logging("../TestParser.log", true);
    //OFLOG_INFO(get_logger(), "We are starting the test!");
    schemaPath = "../schema/tests/test_namechange.json";
    Parser* pObj = new Parser{schemaPath};
    pObj->setDicomDset(testPath);

    OFCondition flag;
    pObj->parse();

    CHECK(pObj->editor.equals("Robert", flag).good());

    //OFLOG_INFO(get_logger(), "We have succeeded! Now end the test!");

    delete pObj;
}

TEST_CASE("Test parsing name change", "[NP]") {
    std::cout<<"We are starting the test!"<<std::endl;
    schemaPath = "../schema/tests/test_namechange.json";
    Parser* pObj = new Parser{schemaPath};
    pObj->setDicomDset(testPath);

    OFCondition flag;
    pObj->parse();

    CHECK(pObj->editor.equals("Robert", flag).good());

    delete pObj;
}

TEST_CASE("Test parsing UInt16 change", "[NP]") {
  schemaPath = "../schema/tests/test_uintchange.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.equals(12, flag).good());

  delete pObj;
}

TEST_CASE("Test parsing Float64 change", "[NP]") {
  schemaPath = "../schema/tests/test_floatchange.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.equals(2000.5, flag).good());

  delete pObj;
}

TEST_CASE("Test parsing AND with REMOVE", "[NP]") {
  schemaPath = "../schema/tests/test_and.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.exists().bad());

  delete pObj;
}

TEST_CASE("Test parsing OR with REMOVE", "[NP]") {
  schemaPath = "../schema/tests/test_or.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.exists().bad());

  delete pObj;
}

TEST_CASE("Test parsing INSERT", "[NP]") {
  schemaPath = "../schema/tests/test_insert.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.exists().good());
  CHECK(pObj->editor.equals(100, flag).good());

  delete pObj;
}

TEST_CASE("Test parsing UPDATE", "[NP]") {
  schemaPath = "../schema/tests/test_update.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.equals(101, flag).good());

  delete pObj;
}

TEST_CASE("Test parsing CLEAR", "[NP]") {
  schemaPath = "../schema/tests/test_clear.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.equals("", flag).good());

  delete pObj;
}

TEST_CASE("Test parsing COPY", "[NP]") {
  schemaPath = "../schema/tests/test_copy.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.equals("John Doe", flag).good());

  delete pObj;
}

TEST_CASE("Test parsing EXIST with APPEND", "[NP]") {
  schemaPath = "../schema/tests/test_existappend.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.equals("John DoeJohn", flag).good());

  delete pObj;
}

TEST_CASE("Test parsing REGEX with PREPEND", "[NP]") {
  
  schemaPath = "../schema/tests/test_regexprepend.json";
  Parser* pObj = new Parser{schemaPath};
  pObj->setDicomDset(testPath);

  OFCondition flag;
  pObj->parse();

  CHECK(pObj->editor.equals("JohnJohn Doe", flag).good());


  delete pObj;
}
