#include <cstdlib>
#include <fstream>
#include <iostream>
#include "json/json.h"


using namespace std;
int main(void) {
  Json::Reader reader;
  Json::Value root;
  ifstream ifs("config.json");
  Json::StyledStreamWriter writer;
  
  std::ofstream outFile;
  
  // Parse JSON and print errors if needed
  if(!reader.parse(ifs, root)) {
    std::cout << reader.getFormattedErrorMessages();
    exit(1);
  } else {
    
    // Read and modify the json data
    std::cout << "Size: " << root.size() << std::endl;
    std::cout << "Contains nums? " << root.isMember("vr") << std::endl;
    root["000.."] = "Code";
    std::cout << root<< std::endl;
    //root["first"] = "Jimmy";
    //root["middle"] = "Danger";
    
    // Write the output to a file
    //outFile.open("output.json");
    //writer.write(outFile, root);
    //outFile.close();    
  }
  return 0;
}