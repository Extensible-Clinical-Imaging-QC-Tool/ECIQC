// Packages to include
//#define CATCH_CONFIG_MAIN
#include "catch.hpp" // C++ Automated Test Cases in a Header
//#include <iostream>
//#include <string>

// dcmtk modules packages to include
//#include <dcmtk/config/osconfig.h>
//#include <dcmtk/dcmdata/dcdatset.h>
//#include <dcmtk/dcmdata/dctk.h>
//#include <dcmtk/dcmnet/dstorscu.h>
//#include <dcmtk/dcmnet/scu.h>
//#include <dcmtk/dcmpstat/dcmpstat.h>
//#include <dcmtk/dcmdata/dcmetinf.h> 

// Classes to include
#include "metadata/validator.hpp"

// DCMTK tests
TEST_CASE("Test for reading in a DICOM image file", "[VT]") {
  DcmFileFormat image;
  OFString patientName;
  OFString uid;
  OFCondition status = image.loadFile("../DICOM_Images/1-1copy.dcm");

  CHECK(status.good() == 1);
  CHECK(image.getDataset()
            ->findAndGetOFString(DCM_PatientName, patientName)
            .good() == true);
  CHECK(
      image.getDataset()->findAndGetOFString(DCM_SOPInstanceUID, uid)
      .good() ==
      true);
  CHECK(uid ==
        "1.3.6.1.4.1.14519.5.2.1.9999.103.2710019309218103448268301702692");
  CHECK(patientName == "James Bond");
}

// Validator class tests
TEST_CASE("Test for validating if a file is DICOM", "[VT]") {
  Validator validate("../DICOM_Images/1-1copy.dcm");
  CHECK(validate.dicomFormatValidation() == OFTrue);
}

TEST_CASE("Test for existence of a tag (file path)", "[VT]") {
  Validator validate("../DICOM_Images/1-1copy.dcm");

  //DCM_PatientName
  DcmTagKey nameTagKey = DcmTagKey(0x0010, 0x0010);
  OFBool validation_test_result = validate.validateTag(nameTagKey);
  //std::cout << validation_test_result << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result == OFTrue);
  CHECK(validate.validation_output == OFTrue);

  OFBool validation_test_result1 = validate.validateTag("PatientName");
  //std::cout << validation_test_result1 << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result1 == OFTrue);
  CHECK(validate.validation_output == OFTrue);
  
  //DCM_PhaseDelay
  DcmTagKey PhaseDelayTagKey = DCM_PhaseDelay;
  OFBool validation_test_result_2 = validate.validateTag(PhaseDelayTagKey);
  //std::cout << validation_test_result_2 << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result_2 == OFFalse);  
  CHECK(validate.validation_output == OFFalse);  
  
  //DCM_PatientName
  OFBool validation_test_result_3 = validate.validateTag(nameTagKey);
  //std::cout << validation_test_result_3 << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result_3 == OFTrue);  
  CHECK(validate.validation_output == OFFalse); 
}

TEST_CASE("Test for existence of a tag (dataset)", "[VT]") {
  DcmFileFormat fileformat;
  DcmDataset *testDset = fileformat.getDataset();
  testDset->putAndInsertString(DCM_PatientName, "John Doe");
  Validator validate(testDset);

  //DCM_PatientName
  DcmTagKey nameTagKey = DcmTagKey(0x0010, 0x0010);
  OFBool validation_test_result = validate.validateTag(nameTagKey);
  //std::cout << validation_test_result << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result == OFTrue);
  CHECK(validate.validation_output == OFTrue);

  OFBool validation_test_result1 = validate.validateTag("PatientName");
  //std::cout << validation_test_result1 << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result1 == OFTrue);
  CHECK(validate.validation_output == OFTrue);
  
  //DCM_PhaseDelay
  DcmTagKey PhaseDelayTagKey = DCM_PhaseDelay;
  OFBool validation_test_result_2 = validate.validateTag(PhaseDelayTagKey);
  //std::cout << validation_test_result_2 << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result_2 == OFFalse);  
  CHECK(validate.validation_output == OFFalse);  
  
  //DCM_PatientName
  OFBool validation_test_result_3 = validate.validateTag(nameTagKey);
  //std::cout << validation_test_result_3 << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result_3 == OFTrue);  
  CHECK(validate.validation_output == OFFalse); 
}

TEST_CASE("Test for existence of a tag with non-empty value (dataset)", "[VT]") {
  DcmFileFormat fileformat;
  DcmDataset *testDset = fileformat.getDataset();
  testDset->putAndInsertString(DCM_PatientName, "John Doe");
  testDset->putAndInsertString(DCM_PhaseDelay, "");
  Validator validate(testDset);

  //DCM_PatientName
  DcmTagKey nameTagKey = DcmTagKey(0x0010, 0x0010);
  OFBool validation_test_result = validate.validateTagValue(nameTagKey);
  //std::cout << validation_test_result << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result == OFTrue);  
  CHECK(validate.validation_output == OFTrue);

  OFBool validation_test_result1 = validate.validateTagValue("PatientName");
  //std::cout << validation_test_result << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result1 == OFTrue);  
  CHECK(validate.validation_output == OFTrue);
  
  //DCM_PhaseDelay
  DcmTagKey PhaseDelayTagKey = DCM_PhaseDelay;
  OFBool validation_test_result_2 = validate.validateTagValue(PhaseDelayTagKey);
  //std::cout << validation_test_result_2 << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result_2 == OFFalse);  
  CHECK(validate.validation_output == OFFalse);  
  
  //DCM_PatientName
  OFBool validation_test_result_3 = validate.validateTagValue(nameTagKey);
  //std::cout << validation_test_result_3 << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result_3 == OFTrue);  
  CHECK(validate.validation_output == OFFalse);    
}

TEST_CASE("Test for existence of a tag with non-empty value and within a range (dataset)", "[VT]") {
  DcmFileFormat fileformat;
  DcmDataset *testDset = fileformat.getDataset();
  testDset->putAndInsertString(DCM_PatientAge, "50");
  Validator validate(testDset);

  //DCM_PatientAge 
  DcmTagKey key = DcmTagKey(0x0010, 0x1010);
  OFBool validation_test_result = validate.validateTagValueRange(key, 0, 100);
  //std::cout << validation_test_result << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result == OFTrue); 
  CHECK(validate.validation_output == OFTrue);

  OFBool validation_test_result1 = validate.validateTagValueRange("PatientAge", 0, 100);
  //std::cout << validation_test_result << std::endl;
  //std::cout << validate.validation_results << std::endl;   
  CHECK(validation_test_result1 == OFTrue); 
  CHECK(validate.validation_output == OFTrue);

  OFBool validation_test_result_2 = validate.validateTagValueRange(key, -100, -10);
  //std::cout << validation_test_result_2 << std::endl;
  //std::cout << validate.validation_results << std::endl;    
  CHECK(validation_test_result_2 == OFFalse); 
  CHECK(validate.validation_output == OFFalse);

  OFBool validation_test_result_3 = validate.validateTagValueRange(key, 0, 100);
  //std::cout << validation_test_result_3 << std::endl;
  //std::cout << validate.validation_results << std::endl;    
  CHECK(validation_test_result_3 == OFTrue); 
  CHECK(validate.validation_output == OFFalse);        
}

TEST_CASE("Test for writing the validation output to the unlimited text field (dataset)", "[VT]") {
  DcmFileFormat fileformat;
  DcmDataset *testDset = fileformat.getDataset();
  testDset->putAndInsertString(DCM_PatientName, "John Doe");
  testDset->putAndInsertString(DCM_PatientSex, "male");
  testDset->putAndInsertString(DCM_PatientAge, "50");
  testDset->putAndInsertString(DCM_PatientWeight, "100");
  testDset->putAndInsertString(DCM_SmokingStatus, "chimney");
  testDset->putAndInsertString(DCM_PhantomType, "Opera");
  testDset->putAndInsertString(DCM_PhaseVector, "");
  Validator validate(testDset);

  // Validate some tags
  validate.validateTag(DCM_PatientName);
  validate.validateTagValue(DCM_PatientSex);
  validate.validateTagValueRange(DCM_PatientAge, 0, 100);
  validate.validateTag(DCM_PhaseDelay);
  validate.validateTagValue(DCM_PhaseVector);
  validate.validateTagValueRange("PatientWeight", 0, 90);
  validate.validateTag(DcmTagKey(0x9999,0x9999));
  validate.validateTagValueRange("misspelledTag", 0, 100);

  // Save the validation output to the unlimited text field
  validate.validationOutputTagWriter();
  
  // Read the unlimited text field of the dataset
  OFString text_value_tag_string;
  validate.dataset->findAndGetOFString(DCM_TextValue, text_value_tag_string);

  //std::cout << "Validation results: \n" << validate.validation_results << std::endl;
  //std::cout << "Text value tag: \n" << text_value_tag_string << std::endl;  

  CHECK(text_value_tag_string == validate.validation_results.c_str()); 
}

TEST_CASE("Test for writing the validation output to a .txt file", "[VT]") {
  DcmFileFormat fileformat;
  DcmDataset *testDset = fileformat.getDataset();
  testDset->putAndInsertString(DCM_PatientName, "John Doe");
  testDset->putAndInsertString(DCM_PatientSex, "male");
  testDset->putAndInsertString(DCM_PatientAge, "50");
  testDset->putAndInsertString(DCM_PatientWeight, "100");
  testDset->putAndInsertString(DCM_SmokingStatus, "chimney");
  testDset->putAndInsertString(DCM_PhantomType, "Opera");
  testDset->putAndInsertString(DCM_PhaseVector, "");
  Validator validate(testDset);

  // Validate some tags
  validate.validateTag(DCM_PatientName);
  validate.validateTagValue(DCM_PatientSex);
  validate.validateTagValueRange(DCM_PatientAge, 0, 100);
  validate.validateTag(DCM_PhaseDelay);
  validate.validateTagValue(DCM_PhaseVector);
  validate.validateTagValueRange("PatientWeight", 0, 90);
  validate.validateTag(DcmTagKey(0x9999,0x9999));
  validate.validateTagValueRange("misspelledTag", 0, 100);

  // Save the validation output to a .txt file
  validate.validationFileWriter();
  
  //std::cout << validate.validation_results << std::endl; 
  
  // Read the output file
  //std::ifstream t("output.txt");
  //std::stringstream buffer;
  //buffer << t.rdbuf();

  std::ifstream ifs("output.txt");
  std::string file_content;

  //ifs.seekg(0, std::ios::end);   
  //file_content.reserve(ifs.tellg());
  //ifs.seekg(0, std::ios::beg);
  
  // Assign the content of the output file to the file_content
  file_content.assign((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

 
  // Convert validation_results into a string for comparison with
  // file_content
  std::string validation_results_string;
  validation_results_string = validate.validation_results.c_str();
 
  CHECK(file_content == validation_results_string);
  //CHECK(buffer.str() == validation_results_string);
} 

TEST_CASE("Testing getting a tag key from tag name", "[VT]") {
  //DCM_PixelData
  DcmTagKey key = Validator::getTagKeyFromDictionary("PixelData");
  //std::cout << "Tag key: \n" << key.toString() << "\n"  << std::endl;
  CHECK(key == DCM_PixelData);  

  // Nonexistent tag name
  DcmTagKey key2 = Validator::getTagKeyFromDictionary("nonsense");
  //std::cout << "Tag key: \n" << key2.toString() << "\n" << std::endl;
  CHECK(key2 == DcmTagKey(0xffff,0xffff));  
}

TEST_CASE("Testing getting a tag name from tag key", "[VT]") {
  //DCM_PixelData
  OFString tag_name = Validator::getTagNameFromDictionary(DCM_PixelData);
  //std::cout << "Tag name: \n" << tag_name << "\n"  << std::endl;
  CHECK(tag_name == "PixelData");  

  // Nonexistent tag key
  DcmTagKey nonsense_key = DcmTagKey(0xffff,0xffff);
  OFString tag_name2 = Validator::getTagNameFromDictionary(nonsense_key);
  //std::cout << "Tag name: \n" << tag_name << "\n"  << std::endl;
  CHECK(tag_name2 == "????"); 
}