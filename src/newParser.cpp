#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "MetadataEditor.hpp"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "newParser.hpp"
#include <regex>

using json = nlohmann::json;

nParser::nParser(OFString configFpath) {

    std::ifstream configFile(configFpath.c_str());
    if (configFile) {
        std::stringstream buffer;
        buffer << configFile.rdbuf();
        configFile.close();

        /* buffer is the stringstream (ss) that contains the config instructions.
            These instructions will be repeated for all the images */
    } else {
        std::cout << "Error reading configuration file";
    }
}


////////////////////////////////////////////////////////////////////
/*                    Public Member Functions                     */
////////////////////////////////////////////////////////////////////


void nParser::setDicomFile(DcmDataset* dset) { 
    currentDataset = dset;
}

DcmDataset* nParser::run() {
    json j;
    configuration >> j;

}


/**< Process settings in order to modify data*/
//TODO What happens to the OFCondition results of the operations
OFBool nParser::pRun() {
    editor = MetadataEditor{currentDataset};
    json base;
    configuration >> base;
    
    // File Level (X level == "Inside a/an X")
    for(const auto& tag: base.items()) {
        OFString otherTagString, newValue;
        OFBool only_overwrite;

        editor.setTag(tag.key())
        

        // Tag Level
        for(const auto& action: tag.values()) {
            switch(action.key()) {
                // Editing options
                case ('INSERT' || 'OVERWRITE'):
                    // Arguments
                    only_overwrite = (action.key() == 'INSERT') ?  OFFalse : OFTrue;
                    newValue = action.values().value;
                    
                    if(action.values().tag == "") {
                        // No 'otherTag' provided there perfom operation on 'thisTag'
                        editor.modify(newValue, only_overwrite);
                    } else if(action.values().tag[0] == "D") {
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

                case 'CLEAR':
                    only_overwrite = OFTrue;
                    newValue = "";

                    if(action.values().tag == "") {
                        editor.modify(newValue, only_overwrite);
                    } else if(action.values().tag[0] == "D") {

                    } else {
                        otherTagString = action.values().tag;
                        editor.modify(newValue, only_overwrite, otherTagString);
                    }

                case 'REMOVE':
                    


                // TODO Complete
                case 'COPY':

                    copyToThis = (action.values().toTag == "") ? OFTrue : OFFalse;
                // Action Level





                // Check options

            }
        }
    }


    }
}


////////////////////////////////////////////////////////////////////
/*                    Private Member Functions                    */
////////////////////////////////////////////////////////////////////






