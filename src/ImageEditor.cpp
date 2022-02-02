#include "ImageEditor.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "mdfdsman.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>
#include <tesseract/baseapi.h>
#include <string>
#include <leptonica/allheaders.h>
#include <regex>

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

void ImageEditor::initTess(){
  api->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
  api->SetPageSegMode(tesseract::PSM_AUTO);

  // Argument '1' refers to bytes per pixel - pre-processed image will be greyscale
  api->SetImage(preProcImage.data, preProcImage.cols, preProcImage.rows, 1, preProcImage.step);
}

// Do we need a separate findText function for testing? Currently the text is checked for blocking
// within the coverText function and findText isn't used
std::string ImageEditor::findText(){
  const char* outText = api->GetUTF8Text();
  foundText = std::string(outText);
  return foundText;
}

// Do we need to distinguish "lettersOnly" with mixed alpha-numeric strings?
OFBool ImageEditor::lettersOnly(std::string text){
  return std::regex_match(text, std::regex("^[A-Za-z]+$"));
}

OFBool ImageEditor::digitsOnly(std::string text){
  return std::all_of(text.begin(), text.end(),
                  [](char c){ return isdigit(c) != 0; });
}

OFBool lessThanFourChars(std::string text){
  return text.length() < 4;
}


/**
 * TODO: Exclusions - text strings to not block/numeric strings to block
 */
// OFBool inExclusions(std::string text){
//   // Check config .json file for exceptions - can be used for letter strings, numerical string (e.g. subject IDs)
//   // and mixed (e.g. "Dose 1" for annotations)
//   // - "Dose 1" might be hard to retain due to tesseract treating words separated by spaces as separate
  
// }

cv::Mat ImageEditor::coverText(){
  Boxa* boxes = api->GetComponentImages(tesseract::RIL_TEXTLINE, true, NULL, NULL);
  //printf("Found %d textline image components.\n", boxes->n);
  
  for (int i = 0; i < boxes->n; i++) {
    BOX* box = boxaGetBox(boxes, i, L_CLONE);

    // "Zoom in" on each bit of text, check it should be blocked, and if so, cover with a rectangle
    api->SetRectangle(box->x, box->y, box->w, box->h);
    std::string ocrResult = api->GetUTF8Text();
    OFBool blockText = OFTrue;

    if(digitsOnly(ocrResult)){
      // TODO: check for exclusions - digit strings to be blocked, otherwise retain them
      blockText = OFFalse;
    } else if(lessThanFourChars(ocrResult)){
      // TODO: check for exclusions - short text/mixed strings to be blocked, otherwise retain them
      blockText = OFFalse;
    } else{
      // TODO: check for exclusions - len>=4 text/mixed strings to be retained instead of automatically blocked
    }

    if(blockText == OFTrue){
      // Draw the rectangle on the original image
      cv::Rect rect(box->x,box->y,box->w,box->h);
      cv::rectangle(datasetImage, rect, cv::Scalar(0, 255, 0));
    }

    boxDestroy(&box);

  }

  return datasetImage;
}

void ImageEditor::endTess(){
  api->End();
  delete api;
}

////////////////////////////////////////////////////////////////////
/*                    Private Member Functions                    */
/////////////////////////////////////////////////////////

OFCondition ImageEditor::prePro(){
  // Convert image to greyscale
  cv::Mat grayImage;
  cv::cvtColor( datasetImage, grayImage, cv::COLOR_BGR2GRAY );
  // Threshold using Otsu
  cv::threshold(grayImage, preProcImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}
