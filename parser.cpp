#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

using namespace std;



class Instances {

    public:
    void function_extractor(); 
 
 
};

void Instances::function_extractor()
{
    ifstream ifs("config.json");
    json j;
    ifs >> j;
    std:: cout << j << '\n';
    vector <json> tags;
    vector <json> values;
    for (const auto& item : j.items())
    {
        
        std::string t = item.key();
        
        tags.push_back(t);


        //std::cout << tags << "\n";
    
        for (const auto& val : item.value().items())
        {
            std::cout << "  " << val.key() << ": " << val.value() << "\n";
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

    Instances obj;
    obj.function_extractor();
    return 0; 

}