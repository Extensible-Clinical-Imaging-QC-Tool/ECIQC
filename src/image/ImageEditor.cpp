#include "ImageEditor.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "metadata/mdfdsman.h"
#include "dcmtk/dcmjpeg/djdecode.h" // decoding JPEG
#include "dcmtk/dcmjpeg/dipijpeg.h"    /* for dcmimage JPEG plugin */
#include "dcmtk/dcmjpeg/djencode.h" // encode JPEGs
#include "dcmtk/dcmjpeg/djrplol.h"
#include "dcmtk/dcmdata/dcpxitem.h"
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
    unsigned short nRows;
    unsigned short nCols;
    unsigned short isSigned;
    long int nImgs {NULL};
    unsigned short bitDepth;
    unsigned short samplesPerPixel;
    unsigned int frameSize;
    unsigned long cvType;
    // copy of dset to be used for decompression
    uncompressedDset = new DcmDataset(*dset);
    DcmElement * pixelElement {NULL};
    DcmPixelData * pixelData {NULL};
    DcmPixelSequence * pixelSequence {NULL};
    OFCondition result {EC_Normal};
    E_TransferSyntax xfer = uncompressedDset->getCurrentXfer();


    // load in data
    uncompressedDset->findAndGetUint16(DCM_BitsAllocated, bitDepth);
    uncompressedDset->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel);
    uncompressedDset->findAndGetUint16(DCM_Rows, nRows);
    uncompressedDset->findAndGetUint16(DCM_Columns, nCols);
    uncompressedDset->findAndGetLongInt(DCM_NumberOfFrames, nImgs);
    uncompressedDset->findAndGetUint16(DCM_PixelRepresentation, isSigned);

    // if NumberOfFrames is 0 then set nImgs to 1 (single frame image)
    if (nImgs==0) {
        nImgs = 1;
    }
    // change representation format to little endian if compressed
    if (xfer!=0 && xfer!=1 && xfer!=2 && xfer!=3) {
        // change to little endian format
        if (! decompressDataset(*uncompressedDset) ) {
            std::cerr << "Error decomporessing dataset of format: " << xfer;
            return false;
        }
    }

    result = uncompressedDset->findAndGetElement(DCM_PixelData, pixelElement);
    if (result.bad() || pixelElement == NULL){
        return false;
    }
    pixelData = OFstatic_cast(DcmPixelData*, pixelElement);
    unsigned long length = pixelData->getLength(uncompressedDset->getCurrentXfer());
    pixelData->getUncompressedFrameSize(uncompressedDset, frameSize);
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
                    Uint8 *buffer = new Uint8[frameSize];
                    pixelData->getUncompressedFrame(uncompressedDset, frame, fragmentStart, buffer, frameSize,
                                                    decompressedColorModel);
                    cv::Mat slice = cv::Mat(nRows, nCols, CV_8UC3, buffer).clone();
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
                    Sint16 *buffer = new Sint16[frameSize];
                    pixelData->getUncompressedFrame(uncompressedDset, frame, fragmentStart, buffer, frameSize,
                                                    decompressedColorModel);
                    cv::Mat slice = cv::Mat(nRows, nCols, CV_16S, buffer).clone();
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
                    Uint16 *buffer = new Uint16[frameSize];
                    pixelData->getUncompressedFrame(uncompressedDset, frame, fragmentStart, buffer, frameSize,
                                                    decompressedColorModel);
                    cv::Mat slice = cv::Mat(nRows, nCols, CV_16U, buffer).clone();
                    // convert to unsigned
                    slices.push_back(slice);
                }
            }
        }

    }
    else{std::cerr << "incompatible channel number"; return false;}
    // delete the uncompressed dset

    // display the original image
    cv::namedWindow( "Unedited Image", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow( "Unedited Image", slices[0]);
    cv::waitKey(0);
    cv::destroyAllWindows();

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
    DcmElement * compressedPixelElement {NULL};
    DcmPixelData * compressedPixelData {NULL};
    dset->findAndGetElement(DCM_PixelData, compressedPixelElement);
    compressedPixelData = OFstatic_cast(DcmPixelData*, compressedPixelElement);

    //Uint8* firstCompressedFrame = getRawJpegData(compressedPixelData);
    // colour images
    if (samplesPerPixel == 3){
        // 8 bit
        if (bitDepth == 8) {
            // unsigned
            if (!isSigned) {
                Uint8 * mergedSlices = combined.data;
                // insert array back into DcmPixelData
                //pixelData->putUint8Array(mergedSlices, length);

            }
        }
    }


        // monochrome images
    else if(samplesPerPixel == 1){
        // 16 bit images
        if (bitDepth == 16) {
            // signed int
            if (isSigned) {

            }
            // unsigned 16 bit
            else {

                }
            }
        }
    return true;
}

void ImageEditor::displayFirstFrame(){
    DJDecoderRegistration::registerCodecs(EDC_photometricInterpretation, EUC_default, EPC_default, false, false, true);

    // create a DicomImage
    DicomImage * image = new DicomImage(dset, dset->getCurrentXfer());
    // gets pixel data, after modality has been applied
    Uint16* pixelData = (Uint16 *)(image->getOutputData(8, 0));
    cv::namedWindow("saved image", cv::WINDOW_AUTOSIZE);
    cv::imshow("saved image", cv::Mat(image->getHeight(), image->getWidth(), CV_8UC3, pixelData ));
    cv::waitKey(0);
    DJDecoderRegistration::cleanup();

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
  api->SetPageSegMode(tesseract::PSM_SPARSE_TEXT);
    for (std::size_t n = 0; n < imageProcessingSlices.size(); n++) {
        // vector of boxes for this slice
        std::vector<Box*> boxesToMask;
        // Argument '1' refers to bytes per pixel - pre-processed image will be greyscale
        api->SetImage(imageProcessingSlices[n].data, imageProcessingSlices[n].cols, imageProcessingSlices[n].rows, 1, imageProcessingSlices[n].step);
        Boxa *boxes = api->GetComponentImages(tesseract::RIL_TEXTLINE, true, NULL, NULL);
        // ensure text has been found
        if (boxes) {
            // loop over each box found
            for (int i = 0; i < boxes->n; i++) {
                BOX *box = boxaGetBox(boxes, i, L_CLONE);

                // "Zoom in" on each bit of text, check it should be blocked, and if so, cover with a rectangle
                api->SetRectangle(box->x, box->y, box->w, box->h);
                std::string ocrResult = api->GetUTF8Text();
                std::cout << ocrResult;
                int conf = api->MeanTextConf();
                OFBool blockText = OFTrue;



//                if (digitsOnly(ocrResult)) {
//                    // TODO: check for exclusions - digit strings to be blocked, otherwise retain them
//                    blockText = OFFalse;
//                } else if (lessThanFourChars(ocrResult)) {
//                    // TODO: check for exclusions - short text/mixed strings to be blocked, otherwise retain them
//                    blockText = OFFalse;
//                } else {
//                    // TODO: check for exclusions - len>=4 text/mixed strings to be retained instead of automatically blocked
//                }

                if (blockText) {
                    // Draw the rectangle on the original image
                    cv::Rect rect(box->x, box->y, box->w, box->h);
                    cv::rectangle(slices[n], rect, cv::Scalar(0, 255, 0));
                    // save the box to vector
                    boxesToMask.push_back(box);
                }

                boxDestroy(&box);

            }
            cv::namedWindow("edited image", cv::WINDOW_AUTOSIZE);
            cv::imshow("edited image", slices[n]);
            cv::waitKey(0);

        }
        else {boxesToMask.push_back(NULL);}
        // add boxes for this slice to the overall vector
        sliceBoxes.push_back(boxesToMask);
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
    DJEncoderRegistration::registerCodecs();
    DJ_RPLossless params; // default params

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
        cv::threshold(imageProcessingSlices[i], imageProcessingSlices[i], 0, 255, cv::THRESH_OTSU);
        //cv::bitwise_not(imageProcessingSlices[i], imageProcessingSlices[i]);

    }
    // Create an average image
    double alpha;
    double beta;
    for (std::size_t i = 0; i < imageProcessingSlices.size(); i++) {
        if (i == 0){averageImage = imageProcessingSlices[0];}
        else {
            alpha = 1.0 / (i + 1);
            beta = 1.0 - alpha;
            cv::addWeighted(imageProcessingSlices[i], alpha, averageImage, beta, 0, averageImage);
        }

    }


    cv::namedWindow( "Average Threshold", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow("Average Threshold", averageImage);
    cv::waitKey(0);

}


