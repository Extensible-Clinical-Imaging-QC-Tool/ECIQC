#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Parser.hpp"
#include "MetadataEditor.cpp"
#include "MetadataEditor.hpp"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <regex>
#include <string>



using json = nlohmann::json;

using namespace std;


/*! \The Parser class parses the configuration file with the user settings. 
*/

void Parser::initial_settings(std::stringstream configuration){

    try
    {
        json j;
        configuration >> j;



        // Check if tag is present 

        if (j.find("0x0001") != j.end()) {
            cout << "this tag key is present " << endl;
        }
        else {
            cout << "this tag key is not present" << endl;
        }


  
        // access operators for checks 
        for (const auto& el : j["0x0001"]["checks"]){
            
            
            if (el.find("EQUAL") != el.end())
            {
                cout << "EQUAL operator is found" << endl;
                for (const auto& s : el["EQUAL"].items())
                {
                    std::cout << s.key() << " : " << s.value() << std::endl;
                    if (s.key() == s.value())
                    {
                        std::cout << "tag equals value" << std::endl;
                    }
                    
                }

            }
            else
            {
                cout << "EQUAL operator not found " << endl;
            }


            if (el.find("EXIST") != el.end())
            {
                cout << "EXIST operator is found" << endl;
                for (const auto& s : el["EXIST"].items())
                {
                    std::cout << s.key() << " : " << s.value() << std::endl;

                    //call EXIST method

                    OFString path = "/home/sabsr3/Documents/Group_Software_Project/ECIQC/"
                    "DICOM_Images/1-1copy.dcm";
                    MetadataEditor obj{path};
                    obj.setTag("(0010,0010)");
                    obj.exists("(0010,0010)");

                    // for example the action in action value is REJECT
                    // call method edit with "REJECT" string input
                    if (el.find("actionIfTrue") != el.end()){
                        for (const auto& v : el["actionifTrue"].items()){
                            //obj.edit(v.value()) // where v.value() = "REJECT"/"REMOVE"/"CLEAR"/"INSERT"/"APPEND"/"OVERWRITE"/"REJECT ALL"
                        }
           

                    } else if(el.find("actionIfFalse") != el.end())
                    {
                        //
                    }
                    else{

                    }

                }

            }
            else
            {
                cout << "EXIST operator not found " << endl;
            }   

            if (el.find("REGEX") != el.end())
            {
                cout << "REGEX operator is found" << endl;
                for (const auto& s : el["EXIST"].items())
                {
                    std::cout << s.key() << " : " << s.value() << std::endl;

                    //call exist method
                    //MetadataEditor obj;
                    //obj.match("0x0001")
                }

            }
            else
            {
                cout << "REGEX operator not found " << endl;
            }   

            // access edit operators for actions     
            
        }
        




        return;/**< process user settings in order to modify data*/
    }
    catch(json::parse_error)
        {
            std::cerr << "parse error " << std::endl;
        }


}


template<class UnaryFunction>
void recursive_iterate(const json& j, UnaryFunction f)
{
    for(auto it = j.begin(); it != j.end(); ++it)
    {
        if (it->is_structured())
        {
            recursive_iterate(*it, f);
        }
        else
        {
            f(it);
        }
    }
}
  