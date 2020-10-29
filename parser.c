#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h> 
using namespace std;
void 
displayCfg(const Json::Value &cfg_root);

int
main()
{
    Json::Reader reader;
    Json::Value cfg_root;
    ifstream ifs("test.json");


    //bool b=reader.parse(ifs, obj);
    // Parse JSON and print errors if needed
    //if(!b) {
    //cout << reader.getFormattedErrorMessages();
    //exit(1);
    //}
    //else {

    reader.parse(ifs, cfg_root);

    std::cout << "__ cfg_root : start __" << std::endl;
    std::cout << cfg_root << std::endl;
    std::cout << "__ cfg_root : end __" << std::endl;

    displayCfg(cfg_root);

    const Json::Value& server = cfg_root["servers"]; // array of characters
   //for ( int index = 0; index < server.size(); ++index )  // Iterates over the sequence elements.

    cout << "quarantine: " << server["quarantine"].asString();
    cout << endl;
    cout << "storage: " << server["storage"].asString();
    cout << endl;
}       
void 
displayCfg(const Json::Value &cfg_root)
{
}