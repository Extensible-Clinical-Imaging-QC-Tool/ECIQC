#include <dcmtk/dcmpstat/dcmpstat.h>


#ifndef WORKERPARAMETERS_H_
#define WORKERPARAMETERS_H_

struct WorkerParameters {
    OFString value, otherTagString, str_expr;

    DcmTagKey otherTagKey;

    OFCondition flag = OFCondition();

    OFBool only_overwrite;
    
    OFBool searchIntoSub = OFFalse;
    OFBool copyToThis = OFFalse;
    OFBool replace = OFTrue;

    unsigned long pos = 0UL;

    unsigned long posFrom = 0UL;
    unsigned long posTo = 0UL;

};

#endif
