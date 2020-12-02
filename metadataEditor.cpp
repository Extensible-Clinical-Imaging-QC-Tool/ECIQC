
#include <iostream>
#include "metadataEditor.hpp"

// Constructor
metadataEditor::metadataEditor(int xx = 23){
    x = xx;
}

// Print function
void metadataEditor::print(){
    std::cout << "x = "<< x << "\n";
}
