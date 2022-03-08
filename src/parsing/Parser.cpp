#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "metadata/MetadataEditor.hpp"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <string>
#include <set>
#include "parsing/Parser.hpp"

/*
 * Logical operators:
 * AND
 * OR
 * NOT
 * GREATER_THAN
 * LESS_THAN
 * EQUALS
 *
 * Actions:
 * UPDATE: Add something in the next value channel without removing current ones
 * INSERT: If tag doesn't exist, insert with specified value
 * OVERWRITE: If tag exists, replace value
 * REMOVE: If tag exists, remove it and its value
 * CLEAR: If tag exists, remove its value
 * PREPEND: String operation, add string at start
 * APPEND: String operation, add string at end
 */

using json = nlohmann::json;
Parser::Parser(){
    std::cout << "REMOVE ME";
}

Parser::Parser(OFString configFpath) {



    
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

void Parser::setDicomDset(DcmDataset* dset) { 
    currentDataset = dset;
}

void Parser::setDicomDset(OFString path) {
    currentDataset = editor.pathToDataset(path);
}

DcmDataset* Parser::getDicomDset() {
    return currentDataset;
}

/* temp function */
DcmDataset* Parser::tempGetDset() {
   return editor.pathToDataset("../tests/test.dcm");
}



//TODO What happens to the OFCondition results of the operations
void Parser::run() {

    int i = 0;
    for(const auto& tag: base.items()) {
        OFString  instruction, sub_instruction;

        std::cout << "One item's key: " << tag.key()  << std::endl;
        std::cout << "One item's value: " << tag.value()  << std::endl;

        editor.setTag(tag.key().c_str());
        std::cout << "Individual tag level: \n" << i << tag.key() << '\t' << base[tag.key()]["operations"] << '\n' << std::endl;
        i++;
        // Tag Level 
        for(const auto& checkList: base[tag.key()]["operations"].items()) {
            for(const auto& action: checkList.value().items()) {
                std::cout <<"\t action : "<< action.key() <<'\n'<< "\t action parameters: " << action.value()<< std::endl;
                parseOperation(action.key().c_str(), action.value());

//                instruction = action.key().c_str();
////              If this is an array, then instruction must be "AND", "OR" or "NOT".
//                if(checkList.value()[action.key()].is_array()) {
//                    /* If true, then the instruction is an operation that must be performed on
//                    the result of all operations specified within this array */
//                    const auto& nested_ops = base[tag.key()]["operations"][0][action.key()][0];
//                    for(const auto& ops: nested_ops.items()) {
//                        std::cout << "\t\t Key: " << ops.key() << '\t\t' << "Value: " << ops.value() << std::endl;
//
//                        sub_instruction = ops.key().c_str();
//
//                        /* TODO call worker to perform the specified action and store the result
//                            Perhaps use  the .size() method to create a vector containing booleans
//                            and then perform the operation on them */
//
//
//                    }
//
////              Otherwise, must be either a check with an action if T/F, or actions with no checks
//                } else {
//
//                    const auto& parameters = base[tag.key()]["operations"][0][action.key()];
//                    //std::cout << instruction << std::endl;
//                    //std::cout << parameters.at("value") << std::endl;
//                    //std::cout << parameters.items().begin().key() << std::endl;
//                    //std::cout << parameters.items().begin().value() << std::endl;
//
//
//                    WorkerParameters paramStruct =  WPMaker(parameters);
////                  Need Worker to differentiate between checks and actions (for IF_TRUE/FALSE)
//                }
//
//                // Create a WorkerParameters object
//                // Call Worker

            }
        }
    }
}

////////////////////////////////////////////////////////////////////
/*                    Private Member Functions                    */
////////////////////////////////////////////////////////////////////



/* An enum containing all possible arguments, required so that the 
    switch loop can be used with string cases */
enum ArgumentsEnum {
    otherTagString,
    otherTagKey,
    value,
    str_expr,
    flag,
    only_overwrite,
    searchIntoSub,
    copyToThis,
    replace,
    posFrom,
    posTo,
    pos,


    tag
};

enum ActionsEnum {
    /* Comparisons */
    EQUAL,
    LESS_THAN,
    GREATER_THAN,
    /* Boolean Operations */
    AND,
    OR,
    NOT,
    /* REGEX */
    EXIST,
    REGEX,
    /* Editing Operations */
    OVERWRITE,
    REMOVE,
    INSERT,
    CLEAR,
    COPY,
    UPDATE,
    APPEND,
    PREPEND,
    /* Objects holding further actions only */
    IF_TRUE,
    IF_FALSE
};

// Maps the arguments in string form to the correct enum
int Parser::resolveArguments(OFString param) {
    static const std::map<OFString, ArgumentsEnum> argStrings {
        {  "otherTagString", otherTagString },
        {  "otherTagKey", otherTagKey  },
        {  "value", value  },
        {  "str_expr", str_expr  },
        {  "flag", flag  },
        {  "only_overwrite", only_overwrite  },
        {  "searchIntoSub",  searchIntoSub  },
        {  "copyToThis",  copyToThis},
        {  "replace",  replace},
        {  "posFrom", posFrom  },
        {  "posTo", posTo  },
        {  "pos", pos  },

        {  "tag", tag }
    };

    auto itr = argStrings.find(param);
    if (itr != argStrings.end()) {
        return itr->second;
    } else {
        return 404;
    }

}

int Parser::resolveActions(OFString param) {
    static const std::map<OFString, ActionsEnum> actionStrings {
        {  "EQUAL", EQUAL  },
        {  "LESS_THAN", LESS_THAN },
        {  "GREATER_THAN", GREATER_THAN  },
        {  "IF_TRUE", IF_TRUE  },
        {  "IF_FALSE", IF_FALSE  },
        {  "AND", AND  },
        {  "OR", OR  },
        {  "NOT", NOT  },
        {  "EXIST",  EXIST  },
        {  "REGEX",  REGEX},
        {  "INSERT", INSERT  },
        {  "REMOVE", REMOVE  },
        {  "CLEAR", CLEAR  },
        {  "COPY", COPY  },
        {  "OVERWRITE", OVERWRITE  },
        {  "UPDATE", UPDATE  },
        {  "APPEND", APPEND  },
        {  "PREPEND", PREPEND  }

    };

    auto itr = actionStrings.find(param);
    if (itr != actionStrings.end()) {
        return itr->second;
    } else {
        return 404;
    }

}

/* Populates the appropriate members of the WorkerParameters struct
in preparation for them to be used as function arguments in worker() */
WorkerParameters Parser::WPMaker(const json& param_object) {

    WorkerParameters paramStruct;

    for(const auto& param: param_object.items()) { 
        OFString argName = OFString(param.key().c_str());
        const auto& arg = param.value();
        //std::cout << "inWorkerParameters\n" << "\t\t\t ARGNAME: " << argName << std::endl;
        
        //std::cout << "\t\t\t ARGVAL: " << arg << std::endl;
        switch (resolveArguments(argName)) {
            /*
            case tag: {
                (tag == "") ? 

                break;
            }
            */
            case otherTagString: {
                /* code */
                paramStruct.otherTagString = OFString(arg.get<std::string>().c_str());
                }
                break;
            case otherTagKey: {
                /* code */
                OFCondition oflag;
                DcmTagKey oTagKey;

                OFString str = OFString(arg.get<std::string>().c_str());
                
                oflag = editor.stringToKey(str, oTagKey);
                if (oflag.bad()) {
                    std::cout << oflag.text() << std::endl;
                }

                paramStruct.otherTagKey = oTagKey;
                }
                break;
            case value: {
                /* code */
                paramStruct.value = OFString(arg.get<std::string>().c_str());
                
                //std::cout << "Get attempt: " << arg.get<std::string>() << '\n';
                //std::cout << "Member: " << paramStruct.value << '\n';
                }
                break;

            case str_expr: {
                /* code */
                paramStruct.str_expr = OFString(arg.get<std::string>().c_str());
                }
                break;
            

            case only_overwrite:
                /* code */
                paramStruct.only_overwrite = arg.get<bool>();
                break;

            case searchIntoSub:
                /* code */
                paramStruct.searchIntoSub = arg.get<bool>();
                break;

            case copyToThis:
                /* code */
                paramStruct.copyToThis = arg.get<bool>();
                break;

            case replace:
                /* code */
                paramStruct.replace = arg.get<bool>();
                break;

            case posFrom:
                /* code */
                paramStruct.posFrom = arg.get<unsigned long>();
                break;

            case posTo/* constant-expression */:
                /* code */
                paramStruct.posTo = arg.get<unsigned long>();
                break;

            case pos:
                /* code */
                paramStruct.pos = arg.get<unsigned long>();
                break;

            case 404: /*Not a possible argument*/
                /* code */
                
                break;

            default:
               break;

            //case /* constant-expression */:
            //    /* code */
            //    break;
        }

    }

    return paramStruct;

}

OFBool Parser::parseTorF(OFBool trueOrFalse, const json& params){
    if (trueOrFalse == OFTrue && params.contains("IF_TRUE")){
        return parseOperation("IF_TRUE", params.at("IF_TRUE"));
    }
    else if (trueOrFalse == OFFalse && params.contains("IF_FALSE")) {
        return parseOperation("IF_FALSE", params.at("IF_FALSE"));
    }
    return OFTrue;
}

// TODO: Add logging of checks and actions performed

OFBool Parser::parseOperation(OFString instruction, const json& params){
    int enumerated_inst = resolveActions(instruction);
    OFBool check_result;
    OFString nested_key;
    json nested_parameters = {};
    WorkerParameters paramStruct;
    switch (enumerated_inst){
        case AND: {
            check_result = OFTrue;
            for(const auto& nested_ops: params.items()){
                nested_key = nested_ops.key().c_str();
                nested_parameters = nested_ops.value();
                if (nested_key != "IF_TRUE" && nested_key != "IF_FALSE") {
                    std::cout << "AND CHECK \t\t Key : " << nested_key << "params : " << nested_parameters << std::endl;
                    if (parseOperation(nested_key, nested_parameters) == OFFalse) {
                        check_result = OFFalse;
                    }
                }
            }
            return parseTorF(check_result, params);
        }
        case OR: {
            check_result = OFFalse;
            for(const auto& nested_ops: params.items()){
                nested_key = nested_ops.key().c_str();
                nested_parameters = nested_ops.value();
                if (nested_key != "IF_TRUE" && nested_key != "IF_FALSE"){
                    std::cout << "OR CHECK \t\t Key : " << nested_key << "params : " << nested_parameters << std::endl;
                    if (parseOperation(nested_key,nested_parameters) == OFTrue){
                        check_result = OFTrue;
                    }
                }
            }
            return parseTorF(check_result, params);
        }
        case NOT: {
            for(const auto& nested_ops: params.items()){
                nested_key = nested_ops.key().c_str();
                nested_parameters = nested_ops.value();
                if (nested_key != "IF_TRUE" && nested_key != "IF_FALSE") {
                    std::cout << "NOT CHECK \t\t Key : " << nested_key << "params : " << nested_parameters << std::endl;
                    check_result = !parseOperation(nested_key, nested_parameters);
                }
            }
            return parseTorF(check_result, params);
        }
        case (IF_TRUE | IF_FALSE): {
            for(const auto& nested_ops: params.items()){
                nested_key = nested_ops.key().c_str();
                nested_parameters = nested_ops.value();
                std::cout << "Key : " << nested_key << "params : " << nested_parameters << std::endl;
                parseOperation(nested_key,nested_parameters);
            }
        }
        case (EQUAL | LESS_THAN | GREATER_THAN | EXIST | REGEX): {
            std::cout << "\t Key : " << instruction << "params : " << params << std::endl;

            paramStruct = WPMaker(params);
            // TODO ... call worker to perform check
            return parseTorF(check_result, params);
        }
        case (INSERT | REMOVE | CLEAR | COPY |
                OVERWRITE | UPDATE | APPEND | PREPEND): {
            std::cout << "\t Key : " << instruction << "params : " << params << std::endl;
            paramStruct = WPMaker(params);
            // TODO ... call worker to perform action
            return OFTrue;
        }
    }
}

/*
OFCondition Parser::worker(int instruction, WorkerParameters params) {


    switch(instruction) {
        // Editing options
        //action.
        case (INSERT || OVERWRITE):
            // Arguments

            //TODO Check that params contains the required arguments for this case

            params.only_overwrite = (resolveActions(instruction.c_str()) == INSERT) ?  OFFalse : OFTrue;
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
        

        case CLEAR: {
            OFBool only_overwrite = OFTrue;
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

        case REMOVE: {
            OFBool all_tags = False;
            OFBool ignore_missing_tags = True;

            editor.deleteTag(tag_path, all_tags, ignore_missing_tags);
            editor.get 

            break;
        }
        // TODO Complete
        case COPY: {
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
    
} 
*/