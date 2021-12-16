#include "mdfdsman.h"
#include "ImageEditor.hpp"

#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>
#include <opencv2/opencv.hpp>

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

////////////////////////////////////////////////////////////////////
/*                    Private Member Functions                    */
////////////////////////////////////////////////////////////////////
OFCondition ImageEditor::prePro(){

}
