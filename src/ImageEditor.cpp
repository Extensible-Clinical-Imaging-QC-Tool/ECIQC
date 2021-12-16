#include "ImageEditor.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "mdfdsman.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>
#include <tesseract/baseapi.h>
#include <string>

// Constructor(s)

ImageEditor::ImageEditor(DcmDataset* dataset) {
  dset = dataset;
}

ImageEditor::ImageEditor(OFString file_path) {
  dset = pathToDataset(file_path);
}

////////////////////////////////////////////////////////////////////
/*                    Public Member Functions                     */
////////////////////////////////////////////////////////////////////

DcmDataset* ImageEditor::pathToDataset(OFString file_path) {
  OFCondition result = loadFile(file_path.c_str());
  if (result.bad()) {
    std::cout << "Error loading file: " << result.text();
  }
  return getDataset();
}

OFCondition ImageEditor::runEditing(){

}

std::string ImageEditor::findText(){
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

  api->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
  api->SetPageSegMode(tesseract::PSM_AUTO);

  // Argument '1' refers to bytes per pixel - pre-processed image will be greyscale
  api->SetImage(preProcImage.data, preProcImage.cols, preProcImage.rows, 1, preProcImage.step);
  const char* outText = api->GetUTF8Text();
  api->End();

  return std::string(outText);
}

char* ImageEditor::getBoxes(){

}

cv::Mat ImageEditor::coverText(){

}

////////////////////////////////////////////////////////////////////
/*                    Private Member Functions                    */
////////////////////////////////////////////////////////////////////
OFCondition ImageEditor::prePro(){

}
