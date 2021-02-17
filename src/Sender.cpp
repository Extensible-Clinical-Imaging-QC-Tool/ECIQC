#include <iostream>
#include <dcmtk/ofstd/ofstd.h> 
#include <dcmtk/dcmnet/dstorscu.h> 
#include "Sender.hpp"


/* Set the parameters

-Create loop to iterate over all filenames

-Then add them to the list of instances to be sent 

-Check for invalid filenames,... and record the logs
*/



/* Create Log */
#define OFFIS_CONSOLE_APPLICATION "dcmsend"

static OFLogger dcmsendLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

Sender::Sender(OFString file_path){

    /*command line parameters */
    OFCommandLine cmd;
    int paramCount = 0;
    paramCount = cmd.getParamCount();

    /* Iterate over input filenames */
    OFList<OFString> inputFiles;
    const char *paramString = NULL;

    for (int i = 3; i <= paramCount; i++){
        cmd.getParam(i,paramString);
        if (OFStandard::dirExists(paramString)){
            OFStandard::searchDirectoryRecursively(paramString,inputFiles);
        }
        else{
            inputFiles.push_back(paramString);
        }
    }

    /* Check if there are input files */
    if (inputFiles.empty())
    {
        OFLOG_FATAL(dcmsendLogger,"input files not found");
        //return EXITCODE_NO_INPUT_FILES;
    }

    DcmStorageSCU storageSCU;
    OFCondition status;

    const char *filename = NULL;
    OFListIterator(OFString) iter = inputFiles.begin();
    OFListIterator(OFString) last = inputFiles.end();

    while (iter != last){
        status = storageSCU.addDicomFile(filename);
        ++iter;
    }


    /*Create log report */
    const char *report_filename = "Sender_Logs";
    status = storageSCU.createReportFile(report_filename);
    if (status.bad()){
        throw "Cannot write to output file";
        //return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
    }

}
