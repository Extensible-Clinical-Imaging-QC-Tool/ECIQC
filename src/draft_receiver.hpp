#ifndef RECEIVER_H
#define RECEIVER_H

#include <dcmtk/dcmnet/scppool.h>
#include <dcmtk/dcmnet/scpthrd.h>
#include "poolbase.h"
/**
 * A worker thread in a multithreaded Service Class Provider. 
 * Runs an association from an already accepted connection.
 */
class ReceiverThread : public DcmThreadSCP
{ OFList<OFString> m_sourcelist;

public:
    /**  Constructor. */
    ReceiverThread();

    /** Destructor. */
    virtual ~ReceiverThread();

    /** Overwrite method of DcmSCP to enable handling of C-STORE requests. */
    OFCondition handleIncomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo);

    
    virtual OFCondition setIPs(const OFList<OFString>& source_list);
    
    /**Check if calling IP is accepted */
    virtual OFBool checkCallingHostAccepted(const OFString& hostOrIP);

};

/**
 * A multithreaded Service Class Provider.
 */
class Receiver : public DQDSCPPool<ReceiverThread, DQDBaseSCPPool, DQDBaseSCPPool::DQDBaseSCPWorker>, public OFThread
{
    //OFList<OFString> m_sourcelist;

public:
    OFCondition result;
    
    /** Constructor. */
    Receiver();

    /** Destructor. */
    virtual ~Receiver();

    /** Stop listening after current association. */
    virtual void request_stop();

    /** Set AE Title. */ // Is this needed??
    void set_name(OFString ae_title);

    void setacceptableIPs(OFList<OFString> source_list);

    //OFList<OFString> getacceptableIPs();

protected:
    /** Overwrite OFThread's run() method. */
    void run();
};
 

#endif