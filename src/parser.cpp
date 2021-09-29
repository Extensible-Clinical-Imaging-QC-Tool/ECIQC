#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Parser.hpp"

using json = nlohmann::json;

using namespace std;


/*! \The Parser class parses the configuration file with the user settings. 
    \It includes basic functionalities for the moment. 
*/
void Parser::initial_settings(){

    
    auto js = R"({"GCC_compiler_version":"9.3.0",
    "warn_return_any":"True",
    "warn_unused_configs":"True",
    "ignore_missing_imports":"True",
    "include_private_tags":"True"})"_json; /**< create a string containing configuration files and convert it to a json object */


    if (js.at("include_private_tags")== true){
        std:: cout << "Proceed including private tags" << '\n';
    }
    else {
        std:: cout << "Proceed removing private tags" << '\n';
    
    }
    return;/**< process user settings ie whether to include or not private tags*/
}



void Parser::function_extractor()
{


    auto js = R"({"GCC_compiler_version":"9.3.0",
    "warn_return_any":"True",
    "warn_unused_configs":"True",
    "ignore_missing_imports":"True",
    "include_private_tags":"True"})"_json;

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

int main(){

    Parser obj2;
    obj2.initial_settings();
 

    Parser obj;
    obj.function_extractor();
}