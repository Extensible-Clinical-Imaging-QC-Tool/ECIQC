#include "ImageEditor.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "mdfdsman.h"
#include "dcmtk/dcmjpeg/djdecode.h" // decoding JPEG
#include "dcmtk/dcmjpeg/dipijpeg.h"    /* for dcmimage JPEG plugin */
#include "dcmtk/dcmjpeg/djencode.h" // encode JPEGs
#include "dcmtk/dcmjpeg/djrplol.h"
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
    DJDecoderRegistration::registerCodecs(EDC_photometricInterpretation, EUC_default, EPC_default, false, false, true);

    // TODO check transfer syntax and handle appropriately

    unsigned short nRows;
    unsigned short nCols;
    long int nImgs;
    unsigned short bitDepth;
    unsigned short samplesPerPixel;
    unsigned long cvType;

    E_TransferSyntax xfer = dset->getCurrentXfer();


    // change representation format to little endian if compressed
  /*  if (xfer!=0 && xfer!=1 && xfer!=2 && xfer!=3) {
        // change to little endian format
        if (!changeDatasetFormat(*dset, EXS_LittleEndianExplicit)){
            std::cerr << "Error decomporessing dataset of format: " << xfer;
            return false;
        }
    }*/


    // load in data
    dset->findAndGetUint16(DCM_BitsAllocated, bitDepth);
    dset->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel);

    // create a DicomImage
    image = new DicomImage(dset, dset->getCurrentXfer());
    // gets pixel data, after modality has been applied
    nRows = image->getHeight();
    nCols = image->getWidth();
    nImgs = image->getFrameCount();
    const DiPixel* pixelData = image->getInterData();
    const void * pixelDataPtr = pixelData->getData();

    // pixels per frame
    const Uint32 frameSize = image->getWidth() * image->getHeight();


    // colour images
    if (!image->isMonochrome()){


        // pixelDataPtr points to array of 3 pointers for each plane (r, g, b)
        const void * const * arrayPixelDataPtr = OFstatic_cast(const void * const *, pixelDataPtr);
        const void *colourPlanes[3] = {NULL, NULL, NULL};
        // pointer to each colour plane
        colourPlanes[0] = arrayPixelDataPtr[0];
        colourPlanes[1] = arrayPixelDataPtr[1];
        colourPlanes[2] =  arrayPixelDataPtr[2];

        switch (pixelData->getRepresentation()){
            case EPR_Uint8:
                for (int frame = 0; frame < nImgs; frame ++ ){
                    // display the output data

                    // load in each plane seperately and then merge, increasing pointer location for each frame
                    Uint8 *r =((Uint8 *) colourPlanes[0]) + frame *frameSize;
                    Uint8 *g = (Uint8 *) colourPlanes[1] + frame *frameSize;
                    Uint8 *b = (Uint8 *) colourPlanes[2] + frame *frameSize;
                    cv::Mat rMat = cv::Mat(nRows, nCols, CV_8UC1, r);
                    cv::Mat gMat = cv::Mat(nRows, nCols, CV_8UC1, g);
                    cv::Mat bMat = cv::Mat(nRows, nCols, CV_8UC1 , b);
                    // OpenCV uses b,g,r
                    auto channels = std::vector<cv::Mat>{bMat, gMat, rMat};
                    // merge the channels together
                    cv::Mat slice;
                    cv::merge(channels, slice);
                    slices.push_back(slice.clone());
                }
                break;

            case EPR_Uint16:
                cvType = CV_16UC3;
                // TODO implement
                break;
            case EPR_Uint32:
                cvType = CV_32FC3;
                // TODO implement
                break;
            default:
                std::cerr << "Unsupported image format";
                return false;
        }
    }
    // monochrome images
    else{
        switch (pixelData->getRepresentation()) {
            case EPR_Uint8:
                // TODO implelement
                cvType = CV_8U;
                break;
            case EPR_Uint16:
                // TODO implement

                break;
            case EPR_Sint16:
                for (int frame = 0; frame < nImgs; frame ++ ){
                    // load in each plane seperately and then merge, increasing pointer location for each frame
                    Uint16 *f = (Uint16 *) pixelDataPtr + frame *frameSize;
                    cv::Mat slice = cv::Mat(nRows, nCols, CV_16S, f).clone();
                    // convert to unsigned
                    slice = slice + 32768;
                    slice.convertTo(slice, CV_16U);
                    slices.push_back(slice);
                }
                break;
            case EPR_Uint32:
                // TODO implement
                cvType = CV_32F;
                break;
            default:
                std::cerr << "Unsupported image format";
                return false;
        }
    }


    DJDecoderRegistration::cleanup();
    // Store first images as the representativeImage
    representativeImage = slices[0].clone();

    if (representativeImage.empty()) {
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
   //savePixelData();
}

// Do we need to distinguish "lettersOnly" with mixed alpha-numeric strings?
OFBool ImageEditor::lettersOnly(std::string text){
  return std::regex_match(text, std::regex("^[A-Za-z]+$"));
}

auto ImageEditor::digitsOnly(std::string text) -> OFBool{
  return std::all_of(text.begin(), text.end(),
                  [](char c){ return isdigit(c) != 0; });
}

OFBool ImageEditor::lessThanFourChars(std::string text){
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
    for (std::size_t i = 0; i < imageProcessingSlices.size(); i++) {
        // Argument '1' refers to bytes per pixel - pre-processed image will be greyscale
        api->SetImage(imageProcessingSlices[i].data, imageProcessingSlices[i].cols, imageProcessingSlices[i].rows, 1, imageProcessingSlices[i].step);
        Boxa *boxes = api->GetComponentImages(tesseract::RIL_TEXTLINE, true, NULL, NULL);
        // ensure text has been found
        if (boxes) {
            for (int i = 0; i < boxes->n; i++) {
                BOX *box = boxaGetBox(boxes, i, L_CLONE);

                // "Zoom in" on each bit of text, check it should be blocked, and if so, cover with a rectangle
                api->SetRectangle(box->x, box->y, box->w, box->h);
                std::string ocrResult = api->GetUTF8Text();
                OFBool blockText = OFTrue;

                if (digitsOnly(ocrResult)) {
                    // TODO: check for exclusions - digit strings to be blocked, otherwise retain them
                    blockText = OFFalse;
                } else if (lessThanFourChars(ocrResult)) {
                    // TODO: check for exclusions - short text/mixed strings to be blocked, otherwise retain them
                    blockText = OFFalse;
                } else {
                    // TODO: check for exclusions - len>=4 text/mixed strings to be retained instead of automatically blocked
                }

                if (blockText) {
                    // Draw the rectangle on the original image
                    cv::Rect rect(box->x, box->y, box->w, box->h);
                    cv::rectangle(slices[i], rect, cv::Scalar(0, 255, 0));
                }

                boxDestroy(&box);

            }
        }
    }
  api->End();
  delete api;
}

const std::vector<cv::Mat> &ImageEditor::getSlices() const {
    return slices;
}

const std::vector<cv::Mat> &ImageEditor::getImageProcessingSlices() const {
    return imageProcessingSlices;
}


////////////////////////////////////////////////////////////////////
/*                    Private Member Functions                    */
/////////////////////////////////////////////////////////

OFBool Image;

OFBool ImageEditor::changeDatasetFormat(DcmDataset &dset, E_TransferSyntax repType=EXS_LittleEndianExplicit) {

    // handle jpeg https://support.dcmtk.org/docs-snapshot/mod_dcmjpeg.html
    //DJDecoderRegistration::registerCodecs(EDC_photometricInterpretation, EUC_default, EPC_default, false, false, true);
    // TODO use chooseRepresentation() to change to uncompressed https://support.dcmtk.org/docs/classDcmDataset.html#a0a857d70d21aa29513f82c1c90eece66
    if(dset.chooseRepresentation(EXS_LittleEndianExplicit, NULL).good() && dset.canWriteXfer(EXS_LittleEndianExplicit) ){
            // force the meta-header UIDs to be re-generated when storing the file
            std::cout << "Compressed image successfully decompressed\n";
            DJDecoderRegistration::cleanup();
            return true;
    }
    //DJDecoderRegistration::cleanup();
    return false;
}

OFBool ImageEditor::changeToOriginalFormat(DcmDataset &dset) {
    DJEncoderRegistration::registerCodecs();
    DJ_RPLossless params; // default params

    if (dset.chooseRepresentation(dset.getOriginalXfer(), &params).good() && dset.canWriteXfer(dset.getOriginalXfer())){
        DJEncoderRegistration::cleanup();
        return true;
    }
    else {
        DJEncoderRegistration::cleanup();
        return false;
    }

}

void ImageEditor::prePro(){

    imageProcessingSlices = slices;

    // loop over slices
    for (std::size_t i = 0; i < imageProcessingSlices.size(); i++) {

        // check if image is already greyscale
        if (!image->isMonochrome()) {
            cv::cvtColor(imageProcessingSlices[i], imageProcessingSlices[i], cv::COLOR_BGR2GRAY );

        }
        // TODO: (maybe) normalise before threshold (TBC - look at tess doc to see  if Tesseract normalises for us)
        // TODO: (maybe) contrast adjustment - look into this (helpful for text detection - again may not be needed for Tesseract
        // convert to 8 bit if not already
        if ( imageProcessingSlices[i].depth() == CV_16U ) {
            cv::normalize(imageProcessingSlices[i], imageProcessingSlices[i], 0., 255., cv::NORM_MINMAX, CV_8UC1);
        }
        cv::threshold(imageProcessingSlices[i], imageProcessingSlices[i], 0, 255, cv::THRESH_OTSU);
    }
    cv::imshow( "Otsu Threshold", imageProcessingSlices[0] );
    cv::waitKey(0);


}


