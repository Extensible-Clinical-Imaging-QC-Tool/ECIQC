#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <iostream>
using namespace std;

class Parser {

    public:
        /** Parser functions:
        *  the initial_settings function processes user choices and calls the editing methods
        */
       void initial_settings(std::stringstream configuration);

};

#endif