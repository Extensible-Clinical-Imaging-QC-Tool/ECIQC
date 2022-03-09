#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include "metadata/MetadataEditor.hpp"
#include "WorkerParameters.hpp"

using json = nlohmann::json;



class Parser {

    public:
        
        
        /** Default constructor for the class. 
         * REMOVE ME
         */
        Parser();


        /** Constructor for the class. A path to a configuration file must be provided
         *
         * @param configFpath 
         */
        Parser(OFString configFpath);

        /** Sets the data member dset using a DcmDataset
         * 
         * @param dset a pointer to a DcmDataset
         */
        void setDicomDset(DcmDataset* dset);

        /** Sets the data member dset using a path to a DICOM file
         * @param path holds the path to the DICOM File
         */
        void setDicomDset(OFString path);

        /** Gets the instance's DICOM dataset
         * 
         * @return DcmDataset* 
         */
        DcmDataset* getDicomDset();

        /** Creates a DcmDataset from DICOM file specified at path
         * 
         * @param path holds the path to a DICOM file
         * @return DcmDataset* 
         */
        DcmDataset* pathToDset(OFString path);

        /** Performs the action specified by instruction using the parameters stored in
         * 'parameters'
         * @param instruction holds the name of the desired action 
         * @param params a struct containing the arguments specified in the config file
         * @return OFCondition which has status EC_Normal if everything is OK, else an error
         */
        OFCondition worker(OFString instruction, WorkerParameters params);


        /** Main loop that parses config file and calls worker to perform the required actions.
         * It also stores the results of each action and congregates results in the case of #
         * boolean operators where the next step is dependent on the result of several actions #
         * the 
         */
        void run();

        /* temp function for testing purposes */
        DcmDataset* tempGetDset();


        // JSON object that holds the config file 
        json base;
        // member that holds the dataset to be modified based on the instructions from the config file
        DcmDataset* currentDataset;
        // object that provides all the methods required for editing
        MetadataEditor editor;
        // Add validator


    private:
        /** Parse results of check IF_TRUE or IF_FALSE
         * @param trueOrFalse OFBool indicating if result of check is true or false, so dictating which actions
         * should follow.
         * @param params json object containing parameters for actions to be performed. Actions may be further nested
         * in these parameters.
         * @return OFBool returns the result of the operation - this will always be true because the check has already
         * been made, so the action will be performed.
         */
        OFCondition parseTorF(OFBool trueOrFalse, const json& params);

        /** Performs an operation based on the parameters passed in from the config .json file.
         *
         * @param instruction string indicating which check or action is to be performed.
         * @param params json object containing parameters for actions to be performed. Actions may be further nested
         * in these parameters.
         * @return OFBool returns the result of the operation - checks can be true or false,
         * actions will always return true after being performed.
         */
        OFCondition parseOperation(OFString instruction, const json& params);

        /** Maps the string form of the actions to the enum. This is required because
         * you can only use integral types as switch cases in switch loops. Therefore 
         * strings can not be used
         * 
         * @param param holds string that needs to be mapped to an enum
         * @return int returns the correct enum member
         */
        int resolveArguments(OFString param);

        /**
         * 
         * @param param holds string that needs to be mapped to an enum
         * @return int returns the correct enum member
         */
        int resolveActions(OFString param);

        /** Function that populates a WorkerParameter struct which will hold all the required arguments for a
         * particular action
         * 
         * @param param_object holds the json object with the desired arguments
         * @return WorkerParameters a populated struct
         */
        WorkerParameters WPMaker(const json& param_object);

        /**
         *
         * @param instruction enumerated instruction indicating which check or action to be performed.
         * @param params WorkerParameters object containing parameters for the check or action to be performed.
         * @return OFCondition with result of check or or action.
         */
        OFCondition worker(int instruction, WorkerParameters params);
};

#endif