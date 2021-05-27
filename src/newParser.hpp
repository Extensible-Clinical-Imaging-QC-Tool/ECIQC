#ifndef NEWPARSER_H_
#define NEWPARSER_H_

#include <string>
#include <iostream>
#include "MetadataEditor.hpp"
//#include <nlohmann/json.hpp>



class nParser {

    public:

        nParser(OFString configFpath);
        void setDicomFile(DcmDataset* dset);

        OFBool pRun(OFString mode);



    private:

        DcmDataset* currentDataset;
};

#endif