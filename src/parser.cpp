#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "MetadataEditor.hpp"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "Parser.hpp"
#include <regex>


using json = nlohmann::json;

using namespace std;


/*! \The Parser class parses the configuration file with the settings. 
*/

    /* file_opening function parses the configuration and raises an error if parsing fails */
    auto Parser::file_opening(std::stringstream configuration){

        try {
            json j;
            configuration >> j;
            return true;
        }
        catch (json::parse_error& e)
        {
            std::cout << "Parse error:" << e.what() << std::endl;
            return false;
        }
        catch (json::out_of_range& e)
        {
            std::cout << "Out of range:" << e.what() << std::endl;
            return false;
        }
        catch (json::exception& e)
        {
            std::cout << "Json exception: " << e.what() << std::endl;
            return false;
        }
    }

    auto Parser::initial_settings(std::stringstream configuration, string Tag, string VR){

        json j;
        configuration >> j;

        // Check if Tag/VR elements are present in the config file


        if (j.find(Tag) != j.end()) {

            cout << "Tag" << Tag << "is present." << endl;
            
            if (j[Tag].find(VR) != j[Tag].end()){

            cout << "VR" << VR << "is present" << endl;
            return true;

            } else{

                cout << "VR" << VR << "is missing" << endl;
                return false;
            }
            
        }
        else {
            cout << "Tag" << Tag << "is not present." << endl;
            return false;
        }

    }

    /**< Process settings in order to modify data*/
    auto Parser::editing(std::stringstream configuration){

        json j;
        configuration >> j;
        
        // Access edit operators and call the corresponding method

        for (const auto& el : j["(0010,0010)"]["checks"]){
           
            OFString name;

            OFString path = "/home/sabsr3/ECIQC/DICOM_Images/1-1copy.dcm";

            MetadataEditor obj{path};

            string main_tag = "(0010,0010)";

            obj.setTag(main_tag.c_str());

            vector<string>op = {"INSERT", "OVERWRITE","REMOVE"}; // vector of actions to perform on the main tag
            
            string tag = el["EXIST"]["tag"]; //access the tag used for the action on the main tag

            string value = el["EXIST"]["value"]; //access the value needed for the action on the main tag

            for (int i = 0; i<op.size(); ++i) 
            {
                // If the tag itself has to be edited, call the modify method without adding the tag key, only with 2 arguments

                if (!el["EXIST"]["tag"].is_null()){

                    if (el["actionIfTrue"]["operator"] == op[i])  //loop through the list of actions to determine the one stated in the config file*/
                    {
                        cout << "Calling method : " << op[i] <<endl;
                        obj.modify(OFString(value.c_str()), OFString(tag.c_str()), false); 
                        string action = "modifying tag using another tag";
                        return action;
                    

                    }

                }
                else{

                    if (el["actionIfTrue"]["operator"] == op[i])  
                    {
                        cout << "Calling method : " << op[i] <<endl;
                        obj.modify(OFString(value.c_str()), false);  
                        string act_variant = "modifying tag itself";
                        return act_variant;
                        
                    }

                }

                
            }


            
            string Tag = tag.c_str();
            int posTo = 0;
            int posFrom = 0;
            OFBool replace = OFFalse;
            OFBool copyToThis = OFTrue;

            // Call copy Tag method 

            if (el["actionIfTrue"]["operator"] == "COPY")
            {
                obj.copyTag(DcmTagKey(Tag[0],Tag[1]), posFrom, posTo, copyToThis, replace);
            }


            else{
                cout << el["actionIfFalse"]["action"] <<endl;
            }


            // REGEX match

            // obj.match(OFString(main_tag.c_str()), OFCondition &OFString(value.c_str()))
            // Raises error wrong expression
            // To do : to input tag key correctly / change to OFString input and perform condition inside the method
   
        }
    }