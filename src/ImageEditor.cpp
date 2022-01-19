#include "ImageEditor.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "mdfdsman.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>
#include <tesseract/baseapi.h>
#include <string>
#include <leptonica/allheaders.h>

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

std::string ImageEditor::findText(){
  const char* outText = api->GetUTF8Text();
  foundText = std::string(outText);
  return foundText;
}

cv::Mat ImageEditor::coverText(){
  Boxa* boxes = api->GetComponentImages(tesseract::RIL_TEXTLINE, true, NULL, NULL);
  //printf("Found %d textline image components.\n", boxes->n);
  
  for (int i = 0; i < boxes->n; i++) {
    BOX* box = boxaGetBox(boxes, i, L_CLONE);

    /* Commented code "zooms in" on a region and prints the text, the confidence tesseract has and the bounding box
    * - could be used for testing?
    //api->SetRectangle(box->x, box->y, box->w, box->h);
    //char* ocrResult = api->GetUTF8Text();
    //int conf = api->MeanTextConf();
    //fprintf(stdout, "Box[%d]: x=%d, y=%d, w=%d, h=%d, confidence: %d, text: %s",
    //                i, box->x, box->y, box->w, box->h, conf, ocrResult);
    */

    // Draw the rectangle on the original image
    cv::Rect rect(box->x,box->y,box->w,box->h);
    cv::rectangle(datasetImage, rect, cv::Scalar(0, 255, 0));

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
////////////////////////////////////////////////////////////////////
/*                    Public Member Functions                     */
///////////////////////////////////////////////////////////////////////////////
OFCondition ImageEditor::prePro(){
  // Convert image to greyscale
  cv::Mat grayImage;
  cv::cvtColor( datasetImage, grayImage, cv::COLOR_BGR2GRAY );
  // Threshold using Otsu
  cv::threshold(grayImage, preProcImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}
