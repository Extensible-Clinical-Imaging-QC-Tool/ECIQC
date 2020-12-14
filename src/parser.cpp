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

    std::ifstream ifs("metadata.json");
    json j = json::parse(ifs);

    // update data
    j.at("00080005") += {"actions", "update"};
    std:: cout << j.at("00080005") << '\n';

    // remove data 
    j.at("00080020") += {"actions", "remove"};
    std:: cout << j.at("00080020") << '\n';

    //clear
    j.at("00080030") += {"actions", "clear"};
    std:: cout << j.at("00080030") << '\n';

    //prepend
    j.at("00080050") += {"actions", "prepend"};
    std:: cout << j.at("00080050") << '\n';
    
    //append
    j.at("00080056") += {"actions", "append"};
    std:: cout << j.at("00080056") << '\n';
    
    //insert 
    j.at("00080061") += {"actions", "insert"};
    std:: cout << j.at("00080061") << '\n';
    
    //overwrite
    j.at("00080090") += {"actions", "overwrite"};
    std:: cout << j.at("00080090") << '\n';

    std::ofstream file("metadata_modify.json");
    file << j;

    return;/**< process user settings in order to modify data*/
}



void Parser::function_extractor()
{

    auto j = R"("00080005"
    )"_json;

    
    vector <json> tags;
    vector <json> values;
    vector <json> er;
  

    for (const auto& item : j.items())
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