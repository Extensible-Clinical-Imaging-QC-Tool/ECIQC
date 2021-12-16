#ifndef IMAGEEDITOR_H_
#define IMAGEEDITOR_H_

#include "mdfdsman.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>
#include <opencv2/opencv.hpp>
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

    OFCondition runEditing();

    OFCondition saveImage();
    /** Runs preprocessed image through tesseract ocr to find text
     * contained
     * @return a string of all text found
     */
    std::string findText();

    /** Takes input of cv::Mat image to find coordinates of text
     * bounding boxes
     * @return a char pointer to a UTF-8 box file
     */
    char* getBoxes();

    /** Uses output of tesseract OCR to draw rectangles on the original
     * image
     * @return an edited cv::Mat image with rectangles over text
     */
    cv::Mat coverText();

private:

   // Holds the dataset to be modified
    DcmDataset *dset;
    // The original dataset image, to be edited and 
    cv::Mat datasetImage;
    // Pre-processed dataset image, to be used for OCR
    cv::Mat preProcImage;
    std::string foundText;

    /** Get the raw pixel data from Utility function
     *  @return Uint8* buffer of pixelData values
     */
    Uint8* mat2PixelData();


    /** Pre-process the DICOM image in order to improve tesseract performance
     *
     * @return OFCondition which has status EC_Normal if everything is OK, else an error
     */
    OFCondition prePro();

    // Writes the processed image back to the DICOM object
    OFCondition writeImage();
};

#endif // ImageEditor_H_