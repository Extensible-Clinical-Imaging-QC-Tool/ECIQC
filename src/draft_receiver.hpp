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
  OFList<OFString> m_peerAETitles;

public:
    /**  Constructor. */
    ReceiverThread();

    /** Destructor. */
    virtual ~ReceiverThread();

    /** Overwrite method of DcmSCP to enable handling of C-STORE requests. */
    OFCondition handleIncomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo);

    
    
    virtual OFCondition setIPs(const OFList<OFString>& source_list);
    virtual OFCondition setpeerAETitles(const OFList<OFString>& peerae_list);
    
    /** Check if calling IP/hostname is accepted by the SCP.
     *  @param hostOrIP Connected peer's hostname or IP.
     *  @return OFTrue if hostname/IP is allowed (or if no acceptable hostnames/IPs are specified).
     *          OFFalse if hostname/IP is not accepted.
     */
    virtual OFBool checkCallingHostAccepted(const OFString& hostOrIP);

};

/**
 * A multithreaded Service Class Provider.
 */
class Receiver : public DQDSCPPool<ReceiverThread, DQDBaseSCPPool, DQDBaseSCPPool::DQDBaseSCPWorker>, public OFThread
{

public:
    OFCondition result;
    
    /** Constructor. */
    Receiver();

    /** Destructor. */
    virtual ~Receiver();

    /** Stop listening after current association. */
    virtual void request_stop();

    /** Set AE Title. 
     *  @param ae_title AE Title to be used by the SCP.
     */ 
    void set_name(OFString ae_title);

    /** Set hostnames/IPs from which SCP can accept data. 
     *  @param source_list A list of strings of acceptable IPs/hostnames.
     */ 
    void setacceptableIPs(OFList<OFString> source_list);

protected:
    /** Overwrite OFThread's run() method. */
    void run();
};
 

#endif