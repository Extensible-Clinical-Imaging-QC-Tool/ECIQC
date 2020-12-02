#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

using namespace std;

// add function to clean text, used it to get the instances, generate standardised file names
// save to nested folders

// dcm::DicomFile dicom_file("path/to/some/dcm");
//if (!dicom_file.Load()) {
  //std::cerr << "Failed to load the file." << std::endl;
  //return;
//}


class Instances {

    public:
        void initial_settings();
        void function_extractor(); 
};

void Instances::initial_settings(){

    //open json config file 
    ifstream ifstr("draft_user_config.json");
    //declare json object and stream from file
    json js;
    ifstr >> js;
    //print parsed file
    //std:: cout << js << '\n';

    // Check user settings 
    if (js.at("include_private_tags")= true){
        std:: cout << "Proceed including private tags" << '\n';
    }
    else {
        std:: cout << "Proceed removing private tags" << '\n';
        //j.erase();
    }
    return;
}



void Instances::function_extractor()
{
    //open json metadata file 
    ifstream ifs("metadata.json");
    //declare json object and stream from file
    json j;
    ifs >> j;

    //print parsed file
    //std:: cout << j << '\n';
    
    vector <json> tags;
    vector <json> values;
    vector <json> er;
  

    for (const auto& item : j.items())
    {

        std::string t = item.key();
        //store the tags 
        tags.push_back(t);
        
    
        for (const auto& val : item.value().items())
        {
            std::cout << "  " << val.key() << ": " << val.value() << "\n";
            // store the tags values
            values.push_back(val.value());
        }

        
    }
    std::ofstream file("tags.json");
    file <<tags;
    std::ofstream file_values("values.json");
    file_values << values;
    return;
              
}
    
int main()
{

    Instances obj2;
    obj2.initial_settings();
 

    Instances obj;
    obj.function_extractor();
    return 0; 
}