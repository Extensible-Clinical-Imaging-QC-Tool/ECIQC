//#ifndef PARSER_H_
//#define PARSER_H_
#pragma once   
#include <string>
#include <iostream>


using namespace std;

class Parser {

    public:
        /** Parser functions:
        *  the initial_settings function processes user choices and calls the editing methods
        */
       auto file_opening(std::stringstream configuration);
       auto initial_settings(std::stringstream configuration, string Tag, string VR);
       auto editing(std::stringstream configuration);


};

//#endif