#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Parser.hpp"

using json = nlohmann::json;

using namespace std;


/*! \The Parser class parses the configuration file with the user settings. 
*/
void Parser::initial_settings(){

    std::ifstream ifs("/home/sabsr3/ECIQC/src/1-1copy.json");
    std::stringstream file_content;
    file_content << ifs.rdbuf();
    json j;
    std::stringstream(file_content.str()) >> j;

    // update data
    j.at("00080008") += {"actions", "update"};

    // remove data 
    j.at("00080016") += {"actions", "remove"};
   
    //clear
    j.at("00080018") += {"actions", "clear"};
    
    //prepend
    j.at("00080020") += {"actions", "prepend"};
    
    //append
    j.at("00080021") += {"actions", "append"};
    
    
    //insert 
    j.at("00080022") += {"actions", "insert"};
    
    
    //overwrite
    j.at("00080023") += {"actions", "overwrite"};
   

    std::ofstream file("1-1copy_modify.json");
    file << j;

    return;/**< process user settings in order to modify data*/
}