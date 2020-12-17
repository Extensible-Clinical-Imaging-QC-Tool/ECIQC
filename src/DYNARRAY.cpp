#include <iostream>
#include <fstream>
#include <new>
#include <string>
#include <nlohmann/json.hpp>
#include "Parser.hpp"
using namespace std;
using json = nlohmann::json;




/*! \
The function table of the Parser class creates a dynamic table containing the tags key, edit type and value used for editing 
M x N matrix
define M to be the number of tags
define N  
*/


// Dynamic Memory Allocation for 2D Array
void Parser::table(std::stringstream configuration)
{


    json j;
    configuration >> j;
    vector <json> tags;
    


    for (const auto& item : j.items())
    {
    std::string t = item.key();
    tags.push_back(t); // store the tags
    }

    int M,N;
    auto size = tags.size();
    M=size;
    N=3;


    // example of edit_types for each tag which should be pre-set by admin

    vector<string> edit_type;
    for (int i = 0; i < M; i++){
        string edit = "N";
        edit_type.push_back(edit);
    }

    edit_type[0] = "update";
    edit_type[1] = "copy";
    edit_type[2] = "insert";
    edit_type[3] = "delete";
    edit_type[4] = "prepend";
    edit_type[5] = "overwrite";
    edit_type[6] = "insert";
    edit_type[7] = "delete";
 

    vector<string> value;

    for (int i = 0; i < M; i++){
        string val = "0";
        value.push_back(val);

    }

    value[0] = "vr";
    value[1] = "0080013";
    value[2] = "1";
    value[3] = " ";
    value[4] = "10";
    value[5] = "0080020";
    value[6] = "5";
    value[7] = " ";

    // dynamically create array of pointers of size M
   
    string** A = new string*[M];

    // dynamically allocate memory of size N for each row
    for (int i = 0; i < M; i++){
      
        A[i] = new string[N];
    }

    // assign values to allocated memory
    
    for (int i = 0; i < M; i++){
       
           A[i][0] = tags[i];
           A[i][1] = edit_type[i];
           A[i][2] = value[i];

    }

    // print the 2D array
    for (int i = 0; i < M; i++)
    {
    for (int j = 0; j < N; j++)
    std::cout << A[i][j] << " ";
    std::cout << std::endl;
    }


    //ofstream array;
    //array.open ("array.txt");


    //for(int i=0;i<M;i++){

        
    //    array << tags[i]; 
    //    array << " ";
    //    array << A[i][1];
    //    array << " ";
    //    array << A[i][2];
    //    array << "\n";

	//}

    //array.close();

    // deallocate memory using delete[] operator
    for (int i = 0; i < M; i++)
    delete[] A[i];
    delete[] A;

    return;
}