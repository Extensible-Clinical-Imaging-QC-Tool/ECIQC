#include <iostream>
#include "MetadataEditor.hpp"

// Constructor
MetadataEditor::MetadataEditor(int xx) {
    x = xx;
}

// Print function
void MetadataEditor::print() {
    std::cout << "x = "<< x << "\n";
}