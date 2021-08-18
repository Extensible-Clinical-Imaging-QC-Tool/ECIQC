#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "MetadataEditor.hpp"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <string>
#include <set>
#include "nParser.hpp"


using json = nlohmann::json;

nParser::nParser(OFString configFpath) {
    std::ifstream configFile(configFpath.c_str());
    if (configFile) {
        std::stringstream buffer;
        buffer << configFile.rdbuf();
        configFile.close();
        /* buffer is the stringstream (ss) that contains the config instructions.
            These instructions will be repeated for all the images */
        buffer >> base;
    } else {
        std::cout << "Error reading configuration file";
    }
}


////////////////////////////////////////////////////////////////////
/*                    Public Member Functions                     */
////////////////////////////////////////////////////////////////////

void nParser::setDicomDset(DcmDataset* dset) { 
    currentDataset = dset;
}

void nParser::setDicomDset(OFString path) {
    currentDataset = editor.pathToDataset(path);
}

DcmDataset* nParser::getDicomDset() {
    return currentDataset;
}

/* temp function */
DcmDataset* nParser::tempGetDset() {
   return editor.pathToDataset("../tests/test.dcm");
}



/*
OFCondition nParser::worker(OFString instruction, WorkerParameters params) {

                                                                    
    switch(hash(instruction.c_str())) {
        // Editing options
        //action.
        case ("INSERT" || "OVERWRITE"): 
            // Arguments

            //TODO Check that params contains the required arguments for this case

            params.only_overwrite = (instruction == "INSERT") ?  OFFalse : OFTrue;
            //params.newValue = action.value().value;

            if(action.values().tag == "") {
                // No 'otherTag' provided therefore perfom operation on 'thisTag'
                editor.modify(params.newValue, params.only_overwrite);
            } else if(action.value().tag[0] == "D") {
                // Tag is provided in word form
                OFString j;
            } else {
                newValue = 4;
                // Tag is provided as (group, element)
                // TODO ensure Tag is in (group,element) form, or take g&e and add (,)
                otherTagString = action.values().tag;
                editor.modify(newValue, only_overwrite, otherTagString); 

            }
            break;
        

        case "CLEAR": {
            only_overwrite = OFTrue;
            newValue = "";

            if(action.values().tag == "") {
                editor.modify(newValue, only_overwrite);
            } else if(action.values().tag[0] == "D") {

            } else {
                otherTagString = action.values().tag;
                editor.modify(newValue, only_overwrite, otherTagString);
            }

            break;
        }

        case "REMOVE": {
            OFBool all_tags = False;
            OFBool ignore_missing_tags = True;

            editor.deleteTag(tag_path, all_tags, ignore_missing_tags);
            editor.get 

            break;
        }
        // TODO Complete
        case "COPY": {
            copyToThis = (action.values().toTag == "") ? OFTrue : OFFalse;
            int posFrom, posTo;

            break;
        }
            //stringstream from(action.values().)
        // Action Level



        default: {
            COUT << "default \n";
        }
        // Check options
    }
    
} */


//TODO What happens to the OFCondition results of the operations
OFBool nParser::pRun() {

    int i = 0;
    for(const auto& tag: base.items()) {
        OFString  instruction, sub_instruction;

        //std::cout << "One item's key: " << tag.key()  << std::endl;
        //std::cout << "One item's value: " << tag.value()  << std::endl;

        editor.setTag(tag.key().c_str());
        std::cout << "Individual tag level: \n" << i << tag.key() << '\t' << base[tag.key()]["checks"] << '\n' << std::endl;
        i++;
        // Tag Level 
        for(const auto& preAction: base[tag.key()]["checks"].items()) {
            for(const auto& action: preAction.value().items()) {
                //std::cout << "\t action : " << action.key()  << std::endl;
                //std::cout << "\t action parameters: " << action.value()/*.items().begin()*/  << std::endl;
            

                instruction = action.key().c_str();
                if(base[tag.key()]["checks"][0][action.key()].is_array()) {
                    /* If true, then the instruction is an operation that must be performed on 
                    the result of all operations specified within this array */

                    const auto& nested_ops = base[tag.key()]["checks"][0][action.key()][0];
                    for(const auto& ops: nested_ops.items()) {
                        //std::cout << "\t\t Key: " << ops.key() << '\t\t' << "Value: " << ops.value() << std::endl;

                        sub_instruction = ops.key().c_str();

                        /* TODO call worker to perform the specified action and store the result
                            Perhaps use  the .size() method to create a vector containing booleans
                            and then perform the operation on them */

                        
                    }
                    
                } else {
                    const auto& parameters = base[tag.key()]["checks"][0][action.key()];
                    std::cout << instruction << std::endl;
                    std::cout << parameters.is_array() << std::endl;
                    //std::cout << parameters.at("value") << std::endl;
                    std::cout << parameters.items().begin().key() << std::endl;
                    std::cout << parameters.items().begin().value() << std::endl;


                    WorkerParameters paramStruct =  WPMaker(parameters);
    
                }
 
                // Create a WorkerParameters object
                // Call Worker

            }
        }
    }
}

// Hash function to allow using the worker switch loop with strings
// TODO Replace this hash method with the enum and map method
constexpr unsigned int hash(const char *s, int off = 0) {                        
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];                           
}     



////////////////////////////////////////////////////////////////////
/*                    Private Member Functions                    */
////////////////////////////////////////////////////////////////////


/* An enum containing all possible arguments, required so that the 
    switch loop can be used with string cases */
enum ArgumentsEnum {
    value,
    otherTagString,
    str_expr,
    otherTagKey,
    flag,
    only_overwrite,
    searchIntoSub,
    copyToThis,
    replace,
    pos
};

// Maps the strings to correct enum
int nParser::resolveArg(OFString param) {

    static const std::map<OFString, ArgumentsEnum> argStrings {
        {  "value", value  },
        {  "otherTagString", otherTagString },
        {  "str_expr", str_expr  },
        {  "otherTagKey", otherTagKey  },
        {  "flag", flag  },
        {  "only_overwrite", only_overwrite  },
        {  "searchIntoSub",  searchIntoSub  },
        {  "copyToThis",  copyToThis},
        {  "replace",  replace},
        {  "pos", pos  }
    };

    auto itr = argStrings.find(param);
    if (itr != argStrings.end()) {
        return itr->second;
    } else {
        return 404;
    }

}

/* Populates the appropriate members of the WorkerParameters struct
in preparation for them to be used as function arguments in worker() */
WorkerParameters nParser::WPMaker(const json& param_object) {

    WorkerParameters paramStruct;
    for(const auto& param: param_object.items()) { 
        OFString argName = OFString(param.key().c_str());
        std::cout << "\t\t\t ARGNAME: " << argName << std::endl ;
        //std::cout << "\t\t\t ARGVAL: " << param.value() << std::endl;
        switch (resolveArg(argName)) {
            default:
                //std::cout << "isString?" << param.value().is_string() <<  "\n\n" << param.value().type_name() << "\n";
                

            case value:
                /* code */
                paramStruct.value = OFString(param.value().get<std::string>().c_str());
                
                std::cout << "Get attempt: " << param.value().get<std::string>() << '\n';
                std::cout << "Member: " << paramStruct.value << '\n';
                break;
            
            case otherTagString:
                /* code */
                paramStruct.otherTagString = OFString(param.value().get<std::string>().c_str());
                break;
            
            case str_expr:
                /* code */
                break;
            
            case otherTagKey:
                /* code */
                break;
            
            case flag:
                /* code */
                break;

            case only_overwrite:
                /* code */
                break;

            case searchIntoSub:
                /* code */
                break;

            case copyToThis:
                /* code */
                break;

            case replace:
                /* code */
                break;

            case pos:
                /* code */
                break;

            case 404: /*Not a possible argument*/
                /* code */
                
                break;

            //case /* constant-expression */:
            //    /* code */
            //    break;
            
        }

    }

    return paramStruct;

} 





