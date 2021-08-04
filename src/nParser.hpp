#ifndef NPARSER_H_
#define NPARSER_H_

#include <string>
#include <iostream>
#include "MetadataEditor.hpp"
#include <nlohmann/json.hpp>


using json = nlohmann::json;



class nParser {

    public:
        
        struct WorkerParameters;
        nParser(OFString configFpath);
        void setDicomDset(DcmDataset* dset);
        void setDicomDset(OFString path);
        DcmDataset*  getDicomDset();
        DcmDataset* pathToDset(OFString path);
        OFCondition worker(OFString instruction, WorkerParameters params);
        OFBool pRun();

        /* temp func */
        DcmDataset* tempDset();


        // Probably privatise later
        json base;
        DcmDataset* currentDataset;
        MetadataEditor editor;


    private:
        int resolveArg(OFString param);
        WorkerParameters WPMaker(const nlohmann::json& param_object);

};

#endif