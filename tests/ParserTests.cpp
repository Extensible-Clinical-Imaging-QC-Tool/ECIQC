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
    OFString schemaPath = "../schema/test_namechange.json";
    Parser* pObj = new Parser{schemaPath};
    pObj->setDicomDset(testPath);

    OFCondition flag;
    pObj->parse();

    CHECK(pObj->editor.equals("Robert", flag).good());

    delete pObj;
}

