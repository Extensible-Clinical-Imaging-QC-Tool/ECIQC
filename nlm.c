#include <iostream>
#include <nlohmann/json.hpp>
  
 using json = nlohmann::json;
  
 int main()
 {
     // create a JSON object
     json j_object = {{"one", 1}, {"two", 2}};
  
     // call erase()
     auto count_one = j_object.erase("one");
     auto count_three = j_object.erase("three");
  
     // print values
     std::cout << j_object << '\n';
     std::cout << count_one << " " << count_three << '\n';

     using integer_t = nlohmann::json::number_integer_t;
     auto ivalue = json.at("val").get<integer_t>();

     if (ivalue < std::numeric_limits<int>::min() || ivalue > std::numeric_limits<int>::max()) {
     // Range error... 
}
 }