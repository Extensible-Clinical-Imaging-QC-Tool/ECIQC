#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "metadata/MetadataEditor.hpp"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <string>
#include <set>
#include "parsing/Parser.hpp"

// Define again???
#define PRV_PrivateQuar DcmTag(0x1333, 0x0052, EVR_IS)

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
// Parser::Parser(){
//     std::cout << "REMOVE ME";
// }

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
    editor.setDset(dset);

}

void Parser::setDicomDset(OFString path) {
    OFCondition result = editor.setDset(path);
    // TODO: Check for error
    currentDataset = editor.dset;
}

DcmDataset* Parser::getDicomDset() {
    return currentDataset;
}


//TODO What happens to the OFCondition results of the operations
DcmDataset* Parser::parse() {
    OFCondition allResults; //If any operation returns bad OFCondition, move to quar
    allResults = EC_Normal;
    int i = 0;
    for(const auto& tag: base.items()) {
        OFString  instruction, sub_instruction;

        std::cout << "One item's key: " << tag.key()  << std::endl;
        std::cout << "One item's value: " << tag.value()  << std::endl;

        editor.setTag(tag.key().c_str());
        std::cout << "\tTag set:\t" << editor.getTagString() << std::endl;
        std::cout << "Individual tag level: \n" << i << tag.key() << '\t' << base[tag.key()]["operations"] << '\n' << std::endl;
        i++;
        // Tag Level 

        for(const auto& action: base[tag.key()]["operations"].items()) {
            //for(const auto& action: checkList.value().items()) {
              std::cout <<"\t action : "<< action.key() <<'\n'<< "\t action parameters: " << action.value()<< std::endl;
              OFCondition actionResult;
              actionResult = parseOperation(action.key().c_str(), action.value(), tag.key().c_str());
              if (actionResult.bad()){
                allResults = makeOFCondition(OFM_dcmdata, 22, OF_error,"Some checks failed");
              }
            //}

        }
    }
    return currentDataset;
}
DcmDataset *Parser::pathToDset(OFString path) { return nullptr; }

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
    tag,
    replaceString,
    compareValue
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
        {  "tag", tag },
        {  "replaceString", replaceString },
        {  "compareValue", compareValue }
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
        std::cout << "inWorkerParameters\n" << "\t\t\t ARGNAME: " << argName << std::endl;
        
        std::cout << "\t\t\t ARGVAL: " << arg << std::endl;
        switch (resolveArguments(argName)) {
            case otherTagString: {
                /* code */
                paramStruct.otherTagString = OFString(arg.get<std::string>().c_str());
                std::cout << "set other tag string\t" << "X" << OFString(arg.get<std::string>().c_str()) << "X" << std::endl;
                std::cout << "set other tag string\t" << "X" << paramStruct.otherTagString.c_str() << "X" << std::endl;
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

            case replaceString:
                /* code */
                paramStruct.replaceString = OFString(arg.get<std::string>().c_str());
                break;

            case compareValue:
                /* code */
                paramStruct.compareValue = arg.get<Float64>();
                break;

            case 404: /*Not a possible argument*/
                /* code */
                
                break;

            default:
               break;
        }

    }

    return paramStruct;

}

OFCondition Parser::parseTorF(OFBool trueOrFalse, const json& params, OFString thisTagString){
    if (trueOrFalse == OFTrue && params.contains("IF_TRUE")){
        return parseOperation("IF_TRUE", params.at("IF_TRUE"), thisTagString);
    }
    else if (trueOrFalse == OFFalse && params.contains("IF_FALSE")) {
        return parseOperation("IF_FALSE", params.at("IF_FALSE"), thisTagString);
    }
    return EC_Normal;
}

// TODO: Add logging of checks and actions performed to output to user (and store in a dicom tag?)
// TODO: Use MetadataEditor for checks and actions

OFCondition Parser::parseOperation(OFString instruction, const json& params,
                                   OFString thisTagString){
    int enumerated_inst = resolveActions(instruction);
    OFBool check_result;
    OFString nested_key;
    json nested_parameters = {};
    WorkerParameters paramStruct;
    std::cout << enumerated_inst << "\t" << instruction << "\n";
    switch (enumerated_inst){
        case AND: {
            check_result = OFTrue;
            for(const auto& nested_ops: params.items()){
                nested_key = nested_ops.key().c_str();
                nested_parameters = nested_ops.value();
                if (nested_key != "IF_TRUE" && nested_key != "IF_FALSE") {
                    std::cout << "AND CHECK \t\t Key : " << nested_key << "params : " << nested_parameters << std::endl;
                    if (parseOperation(nested_key, nested_parameters, thisTagString).bad()) {
                        check_result = OFFalse;
                    }
                }
            }
            return parseTorF(check_result, params, thisTagString);
        }
        case OR: {
            check_result = OFFalse;
            for(const auto& nested_ops: params.items()){
                nested_key = nested_ops.key().c_str();
                nested_parameters = nested_ops.value();
                if (nested_key != "IF_TRUE" && nested_key != "IF_FALSE"){
                    std::cout << "OR CHECK \t\t Key : " << nested_key << "params : " << nested_parameters << std::endl;
                    if (parseOperation(nested_key,nested_parameters, thisTagString).good()){
                        check_result = OFTrue;
                    }
                }
            }
            return parseTorF(check_result, params, thisTagString);
        }
        case NOT: {
            for(const auto& nested_ops: params.items()){
                nested_key = nested_ops.key().c_str();
                nested_parameters = nested_ops.value();
                if (nested_key != "IF_TRUE" && nested_key != "IF_FALSE") {
                    std::cout << "NOT CHECK \t\t Key : " << nested_key << "params : " << nested_parameters << std::endl;
                    check_result = parseOperation(nested_key, nested_parameters, thisTagString).bad();
                }
            }
            return parseTorF(check_result, params, thisTagString);
        }
        case IF_TRUE:
        case IF_FALSE: {
            std::cout << "in if true if false, params:\n" << params <<std::endl;
            for(const auto& nested_ops: params.items()){
                nested_key = nested_ops.key().c_str();
                nested_parameters = nested_ops.value();
                std::cout << "Key : " << nested_key << "params : " << nested_parameters << std::endl;
                parseOperation(nested_key, nested_parameters, thisTagString);
            }
            return EC_Normal; // Doesn't matter what this returns - actions have been done above
        }
        case EQUAL:
        case LESS_THAN:
        case GREATER_THAN:
        case EXIST:
        case REGEX: {
            std::cout << "\t Key : " << instruction << "\n\tparams : " << params << std::endl;

            paramStruct = WPMaker(params);
            std::cout << "\tmake worker params" << std::endl;
            OFCondition check_output = worker(enumerated_inst, paramStruct, thisTagString);
            std::cout << "\trun worker" << std::endl;
            check_result = check_output.good();
            std::cout << "\tcheck result" << check_result << std::endl;
            return parseTorF(check_result, params, thisTagString);
        }
        case INSERT:
        case REMOVE:
        case CLEAR:
        case COPY:
        case OVERWRITE:
        case UPDATE:
        case APPEND:
        case PREPEND: {
            std::cout << "\t Key : " << instruction << "params : " << params << std::endl;
            paramStruct = WPMaker(params);
            OFCondition action_output = worker(enumerated_inst, paramStruct, thisTagString);
            return action_output; // Return normal once actions have been performed
        }
        default: {
            std::cout << "default\n";
            return EC_Normal;
        }
    }
}


OFCondition Parser::worker(int instruction, WorkerParameters params, OFString thisTag) {
    std::cout << "\tstart worker\t" << instruction << "\t" << thisTag << std::endl;
    switch(instruction) {
        //TODO Check that params contains the required arguments for each case

        // INSERT and UPDATE are the same up to the only_overwrite parameter - confirmed by email from Chris
        case INSERT:
        case UPDATE:
            // Arguments
            params.only_overwrite = (instruction == INSERT) ?  OFFalse : OFTrue;
            //params.newValue = action.value().value;

            if(params.otherTagString == "" &&
                params.otherTagKey == DCM_PatientBreedDescription) {
                // No 'otherTag' provided therefore perfom operation on 'thisTag'
                return editor.modify(params.value, params.only_overwrite);
            } else if(params.otherTagKey != DCM_PatientBreedDescription) {
                // Tag is provided in word form
                return editor.modify(params.value, params.otherTagKey, params.only_overwrite);
            } else if (params.otherTagString != "") {
              if (strlen(params.otherTagString.c_str())==11) {
                return editor.modify(params.value, params.otherTagString, params.only_overwrite);
              }
            }
            break;
        case CLEAR: {
            OFBool only_overwrite = OFTrue;
            OFString newValue = NULL;

            if(params.otherTagString == "" &&
                params.otherTagKey == DCM_PatientBreedDescription) {
              return editor.modify(newValue, only_overwrite);
            } else if(params.otherTagKey != DCM_PatientBreedDescription) {
              return editor.modify(newValue, params.otherTagKey, only_overwrite);
            } else if (params.otherTagString != "") {
              return editor.modify(newValue, params.otherTagString, only_overwrite);
            }
            break;
        }

        case REMOVE: {
            std::cout << "\t IN REMOVE\n";
            OFBool all_tags = OFFalse;
            OFBool ignore_missing_tags = OFTrue;

            if(params.otherTagString == "" &&
                params.otherTagKey == DCM_PatientBreedDescription) {
              std::cout << "\t othertagstring blank\n" << thisTag << "\n";
              return editor.deleteTag(thisTag, all_tags, ignore_missing_tags);
            } else if(params.otherTagKey != DCM_PatientBreedDescription) {
              OFString otherTagString = params.otherTagKey.toString();
              return editor.deleteTag(otherTagString, all_tags, ignore_missing_tags);
            } else if (params.otherTagString != "") {
              return editor.deleteTag(params.otherTagString, all_tags, ignore_missing_tags);
            }
            break;
        }
        case COPY: {
          if(params.otherTagString == "" &&
              params.otherTagKey == DCM_PatientBreedDescription) {
            std::cout << "No other tag provided for copy";
            break;
          } else if(params.otherTagKey != DCM_PatientBreedDescription) {
            OFString otherTagString = params.otherTagKey.toString();
            return editor.copy(params.otherTagKey, params.posTo, params.posFrom,
                        params.copyToThis, params.searchIntoSub,params.replace);
          } else if (params.otherTagString != "") {
            return editor.copy(params.otherTagString, params.posTo, params.posFrom,
                        params.copyToThis, params.searchIntoSub,params.replace);
          }
          break;
        }
        case OVERWRITE: {
          if(params.otherTagString == "" &&
              params.otherTagKey == DCM_PatientBreedDescription) {
            return editor.overwrite(params.str_expr, params.replaceString);
          } else if(params.otherTagKey != DCM_PatientBreedDescription) {
            return editor.overwrite(params.otherTagKey, params.str_expr, params.replaceString);
          } else if (params.otherTagString != "") {
            return editor.overwrite(params.otherTagString, params.str_expr, params.replaceString);
          }
          break;
        }
        case APPEND: {
          OFCondition flag;
          if(params.otherTagString == "" &&
              params.otherTagKey == DCM_PatientBreedDescription) {
            return editor.append(params.value, flag, params.pos);
          } else if(params.otherTagKey != DCM_PatientBreedDescription) {
            return editor.append(params.value, params.otherTagKey, flag, params.pos);
          } else if (params.otherTagString != "") {
            return editor.append(params.value, params.otherTagString, flag, params.pos);
          }
          break;
        }
        case PREPEND: {
          OFCondition flag;
          if(params.otherTagString == "" &&
              params.otherTagKey == DCM_PatientBreedDescription) {
            return editor.prepend(params.value, flag, params.pos);
          } else if(params.otherTagKey != DCM_PatientBreedDescription) {
            return editor.prepend(params.value, params.otherTagKey, flag, params.pos);
          } else if (params.otherTagString != "") {
            return editor.prepend(params.value, params.otherTagString, flag, params.pos);
          }
          break;
        }
        case EQUAL: {
          std::cout << "\t\t in EQUAL" << std::endl;
          OFCondition flag;
          if (params.value != ""){
            std::cout << "\t\t Value != blank" << std::endl;
            std::cout << "\t\t Other tag string" << params.otherTagString << std::endl;
            if(params.otherTagString == "" &&
                params.otherTagKey == DCM_PatientBreedDescription) {
              std::cout << "\t\tNo otherTagString" << std::endl;
              std::cout << "\t\tvalue" << params.value << std::endl;
              std::cout << "\t\tpos" << params.pos << std::endl;
              std::cout << "\t\tget tag " << editor.getTagString() << std::endl;
              return editor.equals(params.value, flag, params.pos);
            } else if(params.otherTagKey != DCM_PatientBreedDescription) {
              return editor.equals(params.otherTagKey, params.value, flag, params.pos);
            } else if (params.otherTagString != "") {
              return editor.equals(params.otherTagString, params.value, flag, params.pos);
            }
          }
          else if (params.compareValue != 0){
            if(params.otherTagString == "" &&
                params.otherTagKey == DCM_PatientBreedDescription) {
              return editor.equals(params.compareValue, flag, params.pos);
            } else if(params.otherTagKey != DCM_PatientBreedDescription) {
              return editor.equals(params.otherTagKey, params.compareValue, flag, params.pos);
            } else if (params.otherTagString != "") {
              return editor.equals(params.otherTagString, params.compareValue, flag, params.pos);
            }
          }
          else {
            break;
          }
        }
        // Less or greater than the same, up to a flag to signify which
        case LESS_THAN:
        case GREATER_THAN: {
          std::cout << "\tin less_than\t";
          OFCondition flag;
          OFBool greaterThan = instruction == GREATER_THAN;
          if(params.otherTagString == "" &&
              params.otherTagKey == DCM_PatientBreedDescription) {
            std::cout << "\tno other tag string\n";
            OFCondition temp = editor.greaterOrLessThan(params.compareValue, greaterThan, flag, params.pos);
            std::cout << "\tcondition output" << temp.text() << "\n";
            std::cout << "\t compare value" << params.compareValue << "\n";
            return editor.greaterOrLessThan(params.compareValue, greaterThan, flag, params.pos);
          } else if(params.otherTagKey != DCM_PatientBreedDescription) {
            return editor.greaterOrLessThan(params.otherTagKey, params.compareValue, greaterThan, flag, params.pos);
          } else if (params.otherTagString != "") {
            return editor.greaterOrLessThan(params.otherTagString, params.compareValue, greaterThan, flag, params.pos);
          }
          break;
        }
        case EXIST: {
          if(params.otherTagString == "" &&
              params.otherTagKey == DCM_PatientBreedDescription) {
            return editor.exists(params.searchIntoSub);
          } else if(params.otherTagKey != DCM_PatientBreedDescription) {
            return editor.exists(params.otherTagKey, params.searchIntoSub);
          } else if (params.otherTagString != "") {
            return editor.exists(params.otherTagString, params.searchIntoSub);
          }
        }
        case REGEX:{
          OFCondition flag;
          if(params.otherTagString == "" &&
              params.otherTagKey == DCM_PatientBreedDescription) {
            return editor.match(params.str_expr, flag, params.pos);
          } else if(params.otherTagKey != DCM_PatientBreedDescription) {
            return editor.match(params.otherTagKey, params.str_expr, flag, params.pos);
          } else if (params.otherTagString != "") {
            return editor.match(params.otherTagString, params.str_expr, flag, params.pos);
          }
          break;
        }
        default: {
            COUT << "default\nno action specified";
        }
    }
    return makeOFCondition(OFM_dcmdata, 22, OF_error,"No operation performed");
}

