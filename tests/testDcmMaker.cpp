#include <dcmtk/dcmpstat/dcmpstat.h>

OFCondition makeTestDICOMFile() {
  char uid[100];
  DcmFileFormat fileformat;
  DcmDataset *testDset = fileformat.getDataset();
  testDset->putAndInsertString(DCM_SOPClassUID, UID_SecondaryCaptureImageStorage);
  testDset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));
  testDset->putAndInsertString(DCM_PatientName, "John Doe");
  testDset->putAndInsertString(DCM_RequestedProcedureDescription, "XR CHEST PA AND LATERAL");
  testDset->putAndInsertString(DCM_DetectorManufacturerModelName, "PIXIUM4600");
  testDset->putAndInsertString(DCM_DateOfLastDetectorCalibration, "20111127");
  testDset->putAndInsertOFStringArray(DCM_FieldOfViewOrigin, "0.0\\390.246999999999");
  testDset->putAndInsertOFStringArray(DCM_FilterMaterial, "COPPER\\ALUMINIUM");
  testDset->putAndInsertOFStringArray(DCM_ImagerPixelSpacing, "0.143\\0.143");

  testDset->putAndInsertUint16(DCM_BitsAllocated,16);
  testDset->putAndInsertUint16(DCM_HighBit,11);

  testDset->putAndInsertFloat64(DCM_RealWorldValueIntercept, 2342.03487523);
  testDset->putAndInsertFloat64(DCM_RealWorldValueSlope, 652.9999);
  
  // Other types can be added in a similar fashion

  OFCondition status = fileformat.saveFile("test.dcm", EXS_LittleEndianExplicit);
  if (status.bad()) {
    std::cerr << "Error: cannot write DICOM file (" << status.text() << ")" << std::endl;
  }
  return status;
}

