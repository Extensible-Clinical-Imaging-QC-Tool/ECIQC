#include <dcmtk/dcmpstat/dcmpstat.h>


#ifndef WORKERPARAMETERS_H_
#define WORKERPARAMETERS_H_

struct WorkerParameters {
    OFString value, otherTagString, str_expr, replaceString;
    DcmTagKey otherTagKey = DCM_PatientBreedDescription; //dummy tag for humans
    Float64 compareValue = 0;

    OFCondition flag = OFCondition();

    OFBool only_overwrite;
    
    OFBool searchIntoSub = OFFalse;
    OFBool copyToThis = OFTrue;
    OFBool replace = OFTrue;
    OFBool concatenate = OFTrue;
    OFBool prepend = OFTrue;

    unsigned long pos = 0UL;

    unsigned long posFrom = 0UL;
    unsigned long posTo = 0UL;

};

#endif
