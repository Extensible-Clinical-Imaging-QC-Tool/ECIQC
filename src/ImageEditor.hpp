#ifndef IMAGEEDITOR_H_
#define IMAGEEDITOR_H_

#include "mdfdsman.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
// TODO Update with Elliot's Utility Class
// #include "ImageUtility.h"

class ImageEditor : public MdfDatasetManager {
public:

    /** Constructor to handle the scenario where a path to
     * a DICOM file is provided 
     * @param file_path holds the path to the DICOM file
     */
    ImageEditor(OFString file_path);

    /** Constructor to hand the scenario where a DICOM dataset
     * is passed directly to the class
     * @param dataset holds a pointer to a DICOM dataset
     */
    ImageEditor(DcmDataset* dataset);


/** Creates a DICOM dataset using the DICOM file at the specified
     * path
     * @param file_path holds the path to the DICOM file
     * @return a pointer to the created DcmDataset
     */
    DcmDataset* pathToDataset(OFString file_path);

    void runEditing();

    OFCondition saveImage();

    /**
     * Checks text is all letters. Combined with digitsOnly, can check for a mixture of letters and numbers
     * 
     * @param text text found by tesseract, to be checked for being all letters
     * @return true - no numbers or special characters found
     * @return false - numbers or special characters found
     */
    OFBool lettersOnly(std::string text);

    /**
     * Checks text is all numbers. Combined with lettersOnly, can check for a mixture of letters and numbers
     * 
     * @param text text found by tesseract, to be checked for being all numeric
     * @return true - no letters or special characters found
     * @return false - letters or special characters found
     */
    static OFBool digitsOnly(std::string text);
    
    /**
     * Checks if length of text is less than 4 characters - the length to automatically exclude for all-letter strings
     * 
     * @param text text found by tesseract, length to be checked
     * @return true - length of the text is less than 4 characters
     * @return false - length of text is greater than or equal to 4 characters
     */
    OFBool lessThanFourChars(std::string text);

    /**
     * TODO: Exclusions - text strings to not block/numeric or short strings to block
     */
    // OFBool inExclusions(std::string text);

    /** Uses output of tesseract OCR to draw rectangles on the original
     * image
     * @return an edited cv::Mat image with rectangles over text
     */
    void coverText();

    // The original dataset image, to be edited and
    cv::Mat datasetImage;
private:

    // Function to decompress JPeg datasets
    OFCondition decompressJpegDataset(DcmDataset &dset);

    // Holds the DicomImage instance
    DicomImage *image;
   // Holds the dataset to be modified
    DcmDataset *dset;
    // holds dset for use in editing
    DcmDataset *uncompressedDset;

    // vector of slices for multi slice images
    std::vector <cv::Mat> slices;
    // Pre-processed dataset image, to be used for OCR
    cv::Mat preProcImage;
    // Tesseract API
    tesseract::TessBaseAPI *api;
    // Text identified from the pre-processed image by tesseract
    std::string foundText;

    /** Get the raw pixel data from Utility function
     *  @return Uint8* buffer of pixelData values
     */
    Uint8* mat2PixelData();


    /** Pre-process the DICOM image in order to improve tesseract performance
     *
     * @return OFCondition which has status EC_Normal if everything is OK, else an error
     */
    void prePro();

    // Writes the processed image back to the DICOM object
    OFCondition writeImage();

    bool loadPixelData();
};

#endif // ImageEditor_H_