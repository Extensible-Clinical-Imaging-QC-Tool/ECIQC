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
#include "dcmtk/dcmimgle/dcmimage.h"

// Constructor(s)

ImageEditor::ImageEditor(DcmDataset* dataset) {
  dset = dataset;
  // load the pixel data into image
  loadPixelData();
}

ImageEditor::ImageEditor(OFString file_path) {
    dset = pathToDataset(file_path);
    // load the pixel data into image
    loadPixelData();
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

// Load pixel data from the dset
bool ImageEditor::loadPixelData() {
    image = new DicomImage(dset, EXS_Unknown);

    unsigned int nRows;
    unsigned int nCols;
    unsigned int nImgs;

    // Get the information
    nRows = image->getHeight();
    nCols = image->getWidth();
    nImgs = image->getFrameCount();

    std::vector <cv::Mat> slices(nImgs);

    // Loop for each slice
    for(unsigned int k = 0; k<nImgs; k++){

        Uint16* pixelData = (Uint16 *)(image->getOutputData(16 /* bits */,k /* slice */));

        slices[k] = cv::Mat(nRows, nCols, CV_16U, pixelData).clone();
    }

    // Merge the slices in a single img
    cv::merge(slices,datasetImage);
    if (datasetImage.empty()) {
        return false;
    }
    else {
        return true;
    }

}

auto ImageEditor::runEditing() -> cv::Mat{
    // Preprocess image using thresholding
    prePro();
    // Initiate tesseract
//    initTess();
    //
    return coverText();
}

// Do we need to distinguish "lettersOnly" with mixed alpha-numeric strings?
OFBool ImageEditor::lettersOnly(std::string text){
  return std::regex_match(text, std::regex("^[A-Za-z]+$"));
}

auto ImageEditor::digitsOnly(std::string text) -> OFBool{
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
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  api->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
  api->SetPageSegMode(tesseract::PSM_AUTO);

  // Argument '1' refers to bytes per pixel - pre-processed image will be greyscale
  api->SetImage(preProcImage.data, preProcImage.cols, preProcImage.rows, 1, preProcImage.step);
  Boxa* boxes = api->GetComponentImages(tesseract::RIL_TEXTLINE, true, NULL, NULL);
  
  for (int i = 0; i < boxes->n; i++) {
    BOX* box = boxaGetBox(boxes, i, L_CLONE);

    // "Zoom in" on each bit of text, check it should be blocked, and if so, cover with a rectangle
    api->SetRectangle(box->x, box->y, box->w, box->h);
    std::string ocrResult = api->GetUTF8Text();
    OFBool blockText = OFTrue;

//    if(digitsOnly(ocrResult)){
//      // TODO: check for exclusions - digit strings to be blocked, otherwise retain them
//      blockText = OFFalse;
//    } else if(lessThanFourChars(ocrResult)){
//      // TODO: check for exclusions - short text/mixed strings to be blocked, otherwise retain them
//      blockText = OFFalse;
//    } else{
//      // TODO: check for exclusions - len>=4 text/mixed strings to be retained instead of automatically blocked
//    }

    if(blockText){
      // Draw the rectangle on the original image
      cv::Rect rect(box->x,box->y,box->w,box->h);
      cv::rectangle(datasetImage, rect, cv::Scalar(0, 255, 0));
    }

    boxDestroy(&box);

  }
  api->End();
  delete api;
  return datasetImage;
}

////////////////////////////////////////////////////////////////////
/*                    Private Member Functions                    */
/////////////////////////////////////////////////////////

OFCondition ImageEditor::prePro(){
  // Convert image to greyscale
  cv::Mat grayImage;
  cv::cvtColor( datasetImage, grayImage, cv::COLOR_BGR2GRAY );
  // TODO: (maybe) normalise before threshold (TBC - look at tess doc to see if Tesseract normalises for us)
  // TODO: (maybe) contrast adjustment - look into this (helpful for text detection - again may not be needed for Tesseract)
  // Threshold using Otsu
  cv::threshold(grayImage, preProcImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}

