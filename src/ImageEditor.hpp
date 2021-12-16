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
    /** Default constructor
     */
    ImageEditor();

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



    
private:

    // Holds the dataset to be modified
    DcmDataset *dset;
    // The dataset image
    cv::Mat datasetImage;
    std::string foundText;
}

#endif // ImageEditor_H_