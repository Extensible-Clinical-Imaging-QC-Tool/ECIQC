#include <iostream>
#include "MetadataEditor.hpp"

// Constructor
MetadataEditor::MetadataEditor(OFString file_path, int xx) {
    x = xx;
    OFString fpath = file_path;


}

// Print function
void MetadataEditor::print() {
    std::cout << "x = "<< x << "\n";
}

// Load the file to be edited
void MetadataEditor::readFile() {
    DcmFileFormat image;
    OFCondition status = image.loadFile(fpath);
}