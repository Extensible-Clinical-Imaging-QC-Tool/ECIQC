#include <iostream>
#include "metadataEditor.hpp"


void metadataEditor::print(){
    std::cout << "This member function works \n";
}
metadataEditor::metadataEditor(int xx = 23){
    x = xx;
    std::cout << "In the constructor";
}

/* Constructor that takes in:
 a DICOM file
 a container containing what data fields need editing and in what way*/

