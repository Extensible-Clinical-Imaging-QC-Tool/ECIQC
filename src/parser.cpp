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
        
        // to do: get the values to edit from the table


        // update data

        j.at("00080008") += {"edit_type", "update"};
        j.at("00080008") += {"value", "group element"};

        // remove data 

        j.at("00080016") += {"edit_type", "remove"};
        j.at("00080016") += {"value", " "};

        
        //prepend

        j.at("00080020") += {"edit_type", "prepend"};
        j.at("00080020") += {"value", "001"};

        
        //append

        j.at("00080021") += {"edit_type", "append"};
        j.at("00080021") += {"value", "1"};

        //insert 

        j.at("00080022") += {"edit_type", "insert"};
        j.at("00080022") += {"value", "0001"};

        
        //overwrite

        j.at("00080023") += {"edit_type", "overwrite"};
        j.at("00080023") += {"value", "00080022"};

        // Check if tag is present 

        if (j.find("00080008") != j.end()) {
            cout << "this tag key is present " << endl;
        }
        else {
            cout << "this tag key is not present" << endl;
        }

        // Compare tags 

        for (json::iterator it = j.begin(); it != j.end(); ++it) {
            //std::cout << it.key() << ": "<< it.value() << "\n";
            
            for (const auto& val : it.value().items())
            { 
                    std::string elements = val.key();

                    for (const auto& v : val.value().items())
                { 
                    std::string pre = v.key();

                    //json patch = json::diff(pre[0], pre[1]);
                }

            }



        }




        std::cout << "Enter a tag value: ";

        int x{};

        std::cin >> x;
    
        std::cout << "Enter another tag value: ";

        int y{};

        std::cin >> y;
    
        if (x == y)
            std::cout << x << " equals " << y << '\n';
        if (x != y)
            std::cout << x << " does not equal " << y << '\n';
        if (x > y)
            std::cout << x << " is greater than " << y << '\n';
        if (x < y)
            std::cout << x << " is less than " << y << '\n';
        if (x >= y)
            std::cout << x << " is greater than or equal to " << y << '\n';
        if (x <= y)
            std::cout << x << " is less than or equal to " << y << '\n';





        std::ofstream file("1-1copy_modify.json");
        file << j;

        return;/**< process user settings in order to modify data*/
    }
    catch(json::parse_error)
        {
            std::cerr << "parse error " << std::endl;
        }


}
