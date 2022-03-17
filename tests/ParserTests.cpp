#include "parsing/Parser.hpp"
#include <iostream>
#include "catch.hpp"

DcmTagKey pNameTagKey = DCM_PatientName;
OFString testPath = "test.dcm";
OFString schemaPath = "../schema/useCase.json";
OFString pName;
Parser pObj{schemaPath};


TEST_CASE("Test parser object instantiation","[NP]") {
    std::cout << pObj.base.begin().key();
}

TEST_CASE("Test parser dset setting","[NP]") {
    pObj.setDicomDset(testPath);
    DcmDataset* temp = pObj.currentDataset;
    COUT << temp;
    temp->print(COUT);

}

//TEST_CASE("Test parser parsing","[NP]") {
//    pObj.run();
//}

