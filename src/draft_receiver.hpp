#ifndef RECEIVER_H
#define RECEIVER_H

#include <dcmtk/dcmnet/scppool.h>
#include <dcmtk/dcmnet/scpthrd.h>

/**
 * A worker thread in a multithreaded Service Class Provider. 
 * Runs an association from an already accepted connection.
 */
class ReceiverThread : public DcmThreadSCP
{

public:
    /**  Constructor. */
    ReceiverThread();

    /** Destructor. */
    virtual ~ReceiverThread();

    /** Overwrite method of DcmSCP to enable handling of C-STORE requests. */
    OFCondition handleIncomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo);

};

/**
 * A multithreaded Service Class Provider.
 */
class Receiver : public DcmSCPPool<ReceiverThread>, public OFThread
{

public:
    OFCondition result;
    
    /** Constructor. */
    Receiver();

    /** Destructor. */
    virtual ~Receiver();

    /** Stop listening after current association. */
    virtual void request_stop();

protected:
    /** Overwrite OFThread's run() method. */
    void run();
};
 

#endif