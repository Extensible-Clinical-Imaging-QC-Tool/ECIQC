#include <iostream>
#include <dcmtk/dcmpstat/dcmpstat.h>
#include "MetadataEditor.hpp"

// Constructors
MetadataEditor::MetadataEditor(OFString file_path) {
    OFString fpath = file_path;
}

MetadataEditor::MetadataEditor() {
    x = 23;
}


// Print function
void MetadataEditor::print() {
    std::cout << "x = "<< x << "\n";
}

// Load the file to be edited
void MetadataEditor::readFile() {
    DcmFileFormat image;
    image.loadFile(fpath);
}