#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <dcmtk/dcmpstat/dcmpstat.h>
#include "Parser.hpp"

using json = nlohmann::json;

using namespace std;


/*! \The Parser class parses the configuration file with the user settings. 
*/
void Parser::initial_settings(){

    std::ifstream ifs("1-1copy.json");
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



void Parser::function_extractor()
{

    auto js = R"("00080005"
    )"_json;

    
    vector <json> tags;
    vector <json> values;
    vector <json> er;
  

    for (const auto& item : js.items())
    {

        std::string t = item.key();
        tags.push_back(t); /**< store the tags*/
        
    
        for (const auto& val : item.value().items())
        {
            std::cout << "  " << val.key() << ": " << val.value() << "\n";
            values.push_back(val.value());/**< store the tags values*/
        }

        
    }
    std::ofstream f("output/tags.json");
    f << tags;
    std::ofstream file_values("output/values.json");
    file_values << values;
    return;            
}