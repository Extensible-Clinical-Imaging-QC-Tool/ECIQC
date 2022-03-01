#include "ImageEditor.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "mdfdsman.h"
#include "dcmtk/dcmjpeg/djdecode.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>
#include <tesseract/baseapi.h>
#include <string>
#include <leptonica/allheaders.h>
#include <regex>
#include "dcmtk/dcmimgle/dcmimage.h"
#include<cmath>

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

    // TODO check transfer syntax and handle appropriately
    E_TransferSyntax originalXfer = dset->getOriginalXfer();
    // the uncompressed dset will be the same
    uncompressedDset = new DcmDataset(*dset);
    // If this is not already uncompressed, then uncompress it.
    if ( ! (originalXfer == EXS_LittleEndianImplicit || originalXfer == EXS_BigEndianImplicit || originalXfer == EXS_BigEndianExplicit || originalXfer == EXS_LittleEndianExplicit) ) {
        // the uncompressed dset will be the same
        OFCondition decompressionResult = ImageEditor::decompressJpegDataset(*uncompressedDset);
        // check if succesful
        if (decompressionResult.bad()) {
            std::cerr << "Error in parsing file of transfer syntax: " << originalXfer;
            return false;
        }

    }

    E_TransferSyntax uncompressedXfer = uncompressedDset->getCurrentXfer();
    image = new DicomImage(uncompressedDset, uncompressedXfer);

    unsigned int nRows;
    unsigned int nCols;
    unsigned int nImgs;
    unsigned int bitDepth;
    unsigned int cvType;
    EP_Interpretation colorModel;

    // Get the information
    nRows = image->getHeight();
    nCols = image->getWidth();
    nImgs = image->getFrameCount();
    bitDepth = 16;

    // if it is JPEG, then decompress the dset




    // TODO implement thist to work for different bit depths, will need to look at bits allocated.
    // determine the CV image format
    switch (bitDepth) {
        case 8:
            cvType = CV_8U;
            break;
        case 16:
            cvType = CV_16U;
            break;
    }

    // if it is a single image
    if (nImgs == 1){
        Uint16* pixelData = (Uint16 *)(image->getOutputData(bitDepth));
        datasetImage = cv::Mat(nRows, nCols, CV_16U, pixelData).clone();
    }

    // Loop for each slice
    else {
        for(unsigned int k = 0; k<nImgs; k++){

            Uint16* pixelData = (Uint16 *)(image->getOutputData(16 /* bits */,k /* slice */));

            slices.push_back(cv::Mat(nRows, nCols, CV_16U, pixelData).clone());
        }

        // Store first images as the datasetImage
        datasetImage = slices[0].clone();
    }

    if (datasetImage.empty()) {
        return false;
    }
    else {
        return true;
    }
}

void ImageEditor::runEditing(){
    // Preprocess image using thresholding
    prePro();
   coverText();
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

void ImageEditor::coverText(){
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  api->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
  api->SetPageSegMode(tesseract::PSM_AUTO);

  // Argument '1' refers to bytes per pixel - pre-processed image will be greyscale
  api->SetImage(preProcImage.data, preProcImage.cols, preProcImage.rows, 1, preProcImage.step);
  Boxa* boxes = api->GetComponentImages(tesseract::RIL_TEXTLINE, true, NULL, NULL);
  // ensure text has been found
    if (boxes) {
        for (int i = 0; i < boxes->n; i++) {
            BOX *box = boxaGetBox(boxes, i, L_CLONE);

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

            if (blockText) {
                // Draw the rectangle on the original image
                cv::Rect rect(box->x, box->y, box->w, box->h);
                cv::rectangle(datasetImage, rect, cv::Scalar(0, 255, 0));
            }

            boxDestroy(&box);

        }
    }
  api->End();
  delete api;
}

////////////////////////////////////////////////////////////////////
/*                    Private Member Functions                    */
/////////////////////////////////////////////////////////

OFCondition ImageEditor::decompressJpegDataset(DcmDataset &dset) {

    // handle jpeg https://support.dcmtk.org/docs-snapshot/mod_dcmjpeg.html
    DJDecoderRegistration::registerCodecs();
    // TODO use chooseRepresentation() to change to uncompressed https://support.dcmtk.org/docs/classDcmDataset.html#a0a857d70d21aa29513f82c1c90eece66
    OFCondition result = dset.chooseRepresentation(EXS_LittleEndianImplicit, NULL);
    DJDecoderRegistration::cleanup();
    return result;
}

void ImageEditor::prePro(){

    // Convert image to greyscale
  cv::Mat grayImage = datasetImage;
  // check if image is already greyscale
  //if (!image->isMonochrome()) {
  //    cv::cvtColor(datasetImage, grayImage, cv::COLOR_BGR2GRAY );
  //}
  // TODO: (maybe) normalise before threshold (TBC - look at tess doc to see if Tesseract normalises for us)
  // TODO: (maybe) contrast adjustment - look into this (helpful for text detection - again may not be needed for Tesseract
  // convert to 8 bit
  cv::normalize(grayImage, grayImage, 0.,255., cv::NORM_MINMAX,CV_8UC1);
    cv::imshow( "8 bit Image", grayImage );
    cv::waitKey(0);
  cv::threshold(grayImage, preProcImage, 0, 255, cv::THRESH_OTSU);
  cv::imshow( "8 bit Image", preProcImage );
  cv::waitKey(0);
}

