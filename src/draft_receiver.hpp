#ifndef RECEIVER_H
#define RECEIVER_H

#include <iostream>
#include "dcmtk/config/osconfig.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <dcmtk/dcmnet/scppool.h>
#include <dcmtk/dcmnet/scpthrd.h>

class ReceiverThread : public DcmThreadSCP
{

public:
    ReceiverThread();

    virtual ~ReceiverThread();

    // overwrite method of DcmSCP to disable DcmSCP's standard handler
    OFCondition handleIncomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo);

protected:
    /** Handle incoming storage request.
     
    virtual OFCondition
    handleSTORERequest(T_DIMSE_C_StoreRQ& reqMessage, const T_ASC_PresentationContextID presID, DcmDataset*& reqDataset)
    {
        OFCondition result = receiveSTORERequest(reqMessage, presID, reqDataset);
        // Do whatever you like with dataset, e.g. store to disk to the desired directory
        // ...
        return result;
    } */
};

class StorageServer : public DcmSCPPool<ReceiverThread>, public OFThread
{

public:
    OFCondition result;
    StorageServer();

    virtual ~StorageServer();

    virtual void request_stop();

protected:
    void run(){
        result = DcmSCPPool :: listen();
    };
};
 



#endif