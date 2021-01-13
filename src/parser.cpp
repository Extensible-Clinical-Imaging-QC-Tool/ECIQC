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


  
        // access edit operators and call the corresponding method
        for (const auto& el : j["0x0001"]["checks"]){
            
            
            if (el.find("EQUAL") != el.end())
            {
                cout << "EQUAL operator is found" << endl;
                for (const auto& s : el["EQUAL"].items())
                {
                    std::cout << s.key() << " : " << s.value() << std::endl;
                }
                // call edit method for EQUAL operand

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
                }

            }
            else
            {
                cout << "EXIST operator not found " << endl;
            }       
            
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
  