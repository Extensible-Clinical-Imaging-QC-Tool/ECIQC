#include "ImageEditor.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "metadata/mdfdsman.h"
#include "dcmtk/dcmjpeg/djdecode.h" // decoding JPEG
#include "dcmtk/dcmjpeg/dipijpeg.h"    /* for dcmimage JPEG plugin */
#include "dcmtk/dcmjpeg/djencode.h" // encode JPEGs
#include "dcmtk/dcmjpeg/djrplol.h"
#include "dcmtk/dcmjpeg/djrploss.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include <vector>
#include <tesseract/baseapi.h>
#include <string>
#if TESSERACT_MAJOR_VERSION < 5
    #include <tesseract/strngs.h>
    #include <tesseract/genericvector.h>
#endif
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
    unsigned short nRows;
    unsigned short nCols;
    unsigned short isSigned;
    long int nImgs = -1;
    unsigned short bitDepth;
    unsigned short samplesPerPixel;
    unsigned int frameSize;
    // unsigned long cvType;
    DcmElement * pixelElement {NULL};
    DcmPixelData * pixelData {NULL};
    // DcmPixelSequence * pixelSequence {NULL};
    OFCondition result {EC_Normal};
    E_TransferSyntax xfer = dset->getCurrentXfer();


    // load in data
    dset->findAndGetUint16(DCM_BitsAllocated, bitDepth);
    dset->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel);
    dset->findAndGetUint16(DCM_Rows, nRows);
    dset->findAndGetUint16(DCM_Columns, nCols);
    dset->findAndGetLongInt(DCM_NumberOfFrames, nImgs);
    dset->findAndGetUint16(DCM_PixelRepresentation, isSigned);

    // if NumberOfFrames is 0 then set nImgs to 1 (single frame image)
    if (nImgs==0) {
        nImgs = 1;
    }
    // change representation format to little endian if compressed
    if (xfer!=0 && xfer!=1 && xfer!=2 && xfer!=3) {
        // change to little endian format
        if (! decompressDataset(*dset) ) {
            std::cerr << "Error decomporessing dataset of format: " << xfer;
            return false;
        }
    }

    result = dset->findAndGetElement(DCM_PixelData, pixelElement);
    if (result.bad() || pixelElement == NULL){
        return false;
    }
    pixelData = OFstatic_cast(DcmPixelData*, pixelElement);
    unsigned long length = pixelData->getLength(dset->getCurrentXfer());
    pixelData->getUncompressedFrameSize(dset, frameSize);
    OFString decompressedColorModel;

    // colour images
    if (samplesPerPixel == 3){
        isGrayscale = false;
        // 8 bit
        if (bitDepth == 8) {
            // unsigned
            if (!isSigned) {
                unsigned int fragmentStart{0};
                for (int frame = 0; frame < nImgs; frame++) {

                    std::unique_ptr<Uint8 []> buffer = std::make_unique<Uint8 []>(frameSize);
                    pixelData->getUncompressedFrame(dset, frame, fragmentStart, buffer.get(), frameSize,
                                                    decompressedColorModel);
                    cv::Mat slice = cv::Mat(nRows, nCols, CV_8UC3, buffer.get()).clone();
                    slices.push_back(slice);
                }
            }
        }
    }

    // monochrome images
    else if(samplesPerPixel == 1){
        isGrayscale = true;
        // 16 bit images
        if (bitDepth == 16) {
            // signed int
            if (isSigned) {
                unsigned int fragmentStart{0};
                for (int frame = 0; frame < nImgs; frame++) {

                    std::unique_ptr<Sint16 []> buffer = std::make_unique<Sint16 []>(frameSize);
                    pixelData->getUncompressedFrame(dset, frame, fragmentStart, buffer.get(), frameSize,
                                                    decompressedColorModel);
                    cv::Mat slice = cv::Mat(nRows, nCols, CV_16S, buffer.get()).clone();
                    // convert to unsigned
                    slice = slice + 32768;
                    slice.convertTo(slice, CV_16U);
                    slices.push_back(slice);
                }
            }
            // unsigned 16 bit
            else {
                unsigned int fragmentStart{0};
                for (int frame = 0; frame < nImgs; frame++) {

                    std::unique_ptr<Uint16 []> buffer = std::make_unique<Uint16 []>(frameSize);
                    pixelData->getUncompressedFrame(dset, frame, fragmentStart, buffer.get(), frameSize,
                                                    decompressedColorModel);
                    cv::Mat slice = cv::Mat(nRows, nCols, CV_16U, buffer.get()).clone();
                    // convert to unsigned
                    slices.push_back(slice);
                }
            }
        }

    }
    else{std::cerr << "incompatible channel number"; return false;}

    // display the original image
    //cv::namedWindow( "Unedited Image", cv::WINDOW_AUTOSIZE );// Create a window for display.
    //cv::imshow( "Unedited Image", slices[0]);
    //cv::waitKey(0);
    //cv::destroyAllWindows();

    // run preprocessing step
    prePro();
    // run cover text function
    coverText();


    // save out the image

    // combine slices into single Mat object
    // vertically concatenate to ensure Mat array is sequential
    cv::Mat combined;
    // convert slices vector to array
    cv::vconcat(slices, combined);

    // get JPEG data from compressed dset
    // DcmElement * compressedPixelElement {NULL};
    // DcmPixelData * compressedPixelData {NULL};


    //Uint8* firstCompressedFrame = getRawJpegData(compressedPixelData);
    // colour images
    if (samplesPerPixel == 3){
        // 8 bit
        if (bitDepth == 8) {
            // unsigned
            if (!isSigned) {
                Uint8 * mergedSlices = combined.data;
                // insert array back into DcmPixelData - check if this is actually being done
                pixelData->putUint8Array(mergedSlices, length);

            }
        }
    }

    // monochrome images
    else if(samplesPerPixel == 1){
        // 16 bit images
        if (bitDepth == 16) {
            // signed int
            if (isSigned) {
                // TODO implement
            }
            // unsigned 16 bit
            else {
                // TODO implement
                }
            }
        }

    
    //std::unique_ptr<Uint8 []> buffer = std::make_unique<Uint8 []>(frameSize);
    //unsigned int fragmentStart{0};
    //pixelData->getUncompressedFrame(dset, 0, fragmentStart, buffer.get(), frameSize,
    //                                decompressedColorModel);
    //                cv::Mat slice_new = cv::Mat(nRows, nCols, CV_8UC3, buffer.get()).clone();
    //cv::namedWindow( "edited Image", cv::WINDOW_AUTOSIZE );// Create a window for display.
    //cv::imshow( "edited Image", slice_new);
    //cv::waitKey(0);
    //cv::destroyAllWindows();

    // change back to original (likely jpeg) format
    changeToOriginalFormat(*dset);



    return true;
}

void ImageEditor::displayFirstFrame(){
    DJDecoderRegistration::registerCodecs(EDC_photometricInterpretation, EUC_default, EPC_default, false, false, true);

    // create a DicomImage
    std::unique_ptr<DicomImage> image = std::make_unique<DicomImage>(dset, dset->getCurrentXfer());
    // gets pixel data, after modality has been applied
     Uint16* pixelData = (Uint16 *)(image->getOutputData(8, 0));
    cv::namedWindow("saved image", cv::WINDOW_AUTOSIZE);
    cv::imshow("saved image", cv::Mat(image->getHeight(), image->getWidth(), CV_8UC3, pixelData ));
    cv::waitKey(0);
    DJDecoderRegistration::cleanup();

}

// Do we need to distinguish "lettersOnly" with mixed alpha-numeric strings?
OFBool ImageEditor::lettersOnly(std::string text){
  return std::regex_match(text, std::regex("^[A-Za-z]+$"));
}

auto ImageEditor::digitsOnly(std::string text) -> OFBool{
  return std::all_of(text.begin(), text.end(),
                  [](char c){ return isdigit(c) != 0; });
}

OFBool  ImageEditor::isSpecialCharactersOnly(std::string text){
    return std::regex_match(text, std::regex("[^a-zA-Z0-9]+"));
}


OFBool ImageEditor::lessThanFourChars(std::string text){
    text.erase(std::remove_if(text.begin(), text.end(), isspace), text.end());
    text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());
    std::cout << text << "length: " << text.length() << "\n";

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
    std::unique_ptr<tesseract::TessBaseAPI> api = std::make_unique<tesseract::TessBaseAPI>();
    // Stop tesseract using dictionaries for word recogntion
    // using tesseract generic vectors
#if TESSERACT_MAJOR_VERSION < 5
    GenericVector< STRING > pars_vec;
    GenericVector< STRING > pars_values;
#else
    std::vector< std::string > pars_vec;
    std::vector< std::string > pars_values;
#endif
    
    pars_vec.push_back("load_system_dawg");
    pars_vec.push_back("load_freq_dawg");

    pars_values.push_back("0");
    pars_values.push_back("0");

    api->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY, NULL, 0, &pars_vec, &pars_values, false);
    api->SetPageSegMode(tesseract::PSM_SPARSE_TEXT);

    // Argument '1' refers to bytes per pixel - pre-processed image will be greyscale
    api->SetImage(averageImage.data, averageImage.cols, averageImage.rows, 1, averageImage.step);
    Boxa *boxes = api->GetComponentImages(tesseract::RIL_TEXTLINE, true, NULL, NULL);
    // ensure text has been found
    if (boxes) {
        // loop over each box found
        for (int i = 0; i < boxes->n; i++) {

            BOX *box = boxaGetBox(boxes, i, L_CLONE);

            // "Zoom in" on each bit of text, check it should be blocked, and if so, cover with a rectangle
            api->SetRectangle(box->x, box->y, box->w, box->h);
            std::unique_ptr<std::string> ocrResultPtr = std::make_unique<std::string>(api->GetUTF8Text());
            OFBool blockText = OFTrue;

            // checks on text content
            // check if only blank space
            if (ocrResultPtr->find_first_not_of(" \\t\\n\\v\\f\\r") == std::string::npos) {blockText = OFFalse;}
            // check if empty
            else if (*ocrResultPtr == "") {blockText = OFFalse;}
            // check if only special characters
            else if (isSpecialCharactersOnly(*ocrResultPtr)) {blockText = OFFalse;}

            else if (lessThanFourChars(*ocrResultPtr)) {
                blockText = OFFalse;
            }

            if (blockText) {
                std::cout << *ocrResultPtr << "location x:" << box->x << " y: " << box->y << "\n";
                cv::Rect rect(box->x, box->y, box->w, box->h);
                // Draw the rectangle on the original image for each slice
                for (std::size_t n = 0; n < slices.size(); n++) {
                    cv::rectangle(slices[n], rect, cv::Scalar(0, 255, 0), cv::FILLED);
                }
            }
            boxDestroy(&box);
        }
    }
    cv::imwrite("../presentation/otsuText.png", slices[0]);
    //TODO add image back into dicom file here- may have already been written but unsure

    api->End();
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


OFBool ImageEditor::decompressDataset(DcmDataset &dataset) {

    // handle jpeg https://support.dcmtk.org/docs-snapshot/mod_dcmjpeg.html
    if(dataset.chooseRepresentation(EXS_LittleEndianExplicit, NULL).good() && dataset.canWriteXfer(EXS_LittleEndianExplicit) ){
            // force the meta-header UIDs to be re-generated when storing the file
            std::cout << "Compressed image successfully decompressed\n";
            return true;
    }
    return false;
}

Uint8* ImageEditor::getRawJpegData(DcmPixelData* pixelData){
    DcmPixelSequence *dseq = NULL;
    E_TransferSyntax xferSyntax = EXS_Unknown;
    const DcmRepresentationParameter *rep = NULL;
    // Find the key that is needed to access the right representation of the data within DCMTK
    pixelData->getOriginalRepresentationKey(xferSyntax, rep);
    // Access original data representation and get result within pixel sequence
    OFCondition result = pixelData->getEncapsulatedRepresentation(xferSyntax, rep, dseq);
    Uint8* pixData = NULL;
    if ( result == EC_Normal )
    {
        DcmPixelItem* pixitem = NULL;
        // Access first frame (skipping offset table)
        dseq->getItem(pixitem, 1);
        if (pixitem == NULL)
            return NULL;
        // Get the length of this pixel item (i.e. fragment, i.e. most of the time, the length of the frame)
        Uint32 length = pixitem->getLength();
        if (length == 0)
            return NULL;
        // Finally, get the compressed data for this pixel item
        result = pixitem->getUint8Array(pixData);
    }
    if (result.bad())
        return NULL;
    else
        return pixData;
}

OFBool ImageEditor::changeToOriginalFormat(DcmDataset &dataset) {
    // if we want to use JPEG-lS https://support.dcmtk.org/docs/mod_dcmjpls.html

    DJEncoderRegistration::registerCodecs();
    DJ_RPLossless params;
    if (dataset.chooseRepresentation(dataset.getOriginalXfer(), &params).good() && dataset.canWriteXfer(dataset.getOriginalXfer())){
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
        if (!isGrayscale) {
            cv::cvtColor(imageProcessingSlices[i], imageProcessingSlices[i], cv::COLOR_BGR2GRAY );

        }
        // TODO: (maybe) normalise before threshold (TBC - look at tess doc to see  if Tesseract normalises for us)
        // TODO: (maybe) contrast adjustment - look into this (helpful for text detection - again may not be needed for Tesseract
        // convert to 8 bit if not already
        if ( imageProcessingSlices[i].depth() == CV_16U ) {
            cv::normalize(imageProcessingSlices[i], imageProcessingSlices[i], 0., 255., cv::NORM_MINMAX, CV_8UC1);
        }
        // commented out 14122022 as it is currently unused as far as we are aware
        // double C {2}; 
        cv::bitwise_not(imageProcessingSlices[i], imageProcessingSlices[i]);
        cv::threshold(imageProcessingSlices[i], imageProcessingSlices[i], 0, 255, cv::THRESH_OTSU);

    }

    // Create an average image
//    for (std::size_t i = 0; i < imageProcessingSlices.size(); i++) {
//        if (i == 0){averageImage = imageProcessingSlices[0];}
//        else {
//            cv::bitwise_and(averageImage, imageProcessingSlices[i], averageImage);
//        }
//    }
    averageImage = imageProcessingSlices[0];
    cv::imwrite("../presentation/otsuThresh.png", imageProcessingSlices[0]);


//    cv::namedWindow( "Average Threshold", cv::WINDOW_AUTOSIZE );// Create a window for display.
//    cv::imshow("Average Threshold", averageImage);
//    cv::waitKey(0);

}


