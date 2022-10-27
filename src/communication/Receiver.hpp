#ifndef RECEIVER_H
#define RECEIVER_H

#include <dcmtk/dcmnet/scppool.h>
#include <dcmtk/dcmnet/scpthrd.h>
#include "poolbase.h"
#include "ThreadSafeQueue.hpp"
/**
 * A worker thread in a multithreaded Service Class Provider. 
 * Runs an association from an already accepted connection.
 */
class ReceiverThread : public DcmThreadSCP
{ OFList<OFString> m_sourcelist;
  OFList<OFString> m_peerAETitles;
  OFshared_ptr<ThreadSafeQueue<DcmDataset>> m_dset; 
  OFMutex m_edit_dset;

public:
    /**  Constructor. */
    ReceiverThread();

    /** Destructor. */
     ~ReceiverThread() override;

    /** Overwrite method of DcmSCP to enable handling of C-STORE requests. */
    OFCondition handleIncomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo) override;

    /** Set IP/hostnames accepted by the SCP worker.
     *  @param source_list List of accepted hostname or IP.
     */
    virtual OFCondition setIPs(const OFList<OFString>& source_list);

    /** Set called AE titles accepted by the SCP worker.
     *  @param peerae_list List of accepted called AE Titles.
     */
    virtual OFCondition setpeerAETitles(const OFList<OFString>& peerae_list);
    
    /** Check if calling IP/hostname is accepted by the SCP.
     *  @param hostOrIP Connected peer's hostname or IP.
     *  @return OFTrue if hostname/IP is allowed (or if no acceptable hostnames/IPs are specified).
     *          OFFalse if hostname/IP is not accepted.
     */
     OFBool checkCallingHostAccepted(const OFString& hostOrIP) override;

    /** Check if calling AE Title is accepted by the SCP.
     *  @param callingAE Connected peer's AE Title.
     *  @return OFTrue if calling AE Title is allowed (or if no acceptable calling AE Titles are specified).
     *          OFFalse if calling AE is not accepted.
     */
     OFBool checkCallingAETitleAccepted(const OFString& callingAE) override; 
    
    /** Set a shared pointer for storing received DICOM images.
     *  @param dset Shared pointer to be used by the worker.
     */
    void setdatasetaddress(OFshared_ptr<ThreadSafeQueue<DcmDataset>> dset);


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
    Receiver(Uint16 port, std::string aetitle);
    Receiver(const Receiver &) = delete;
    Receiver(Receiver &&) = delete;
    Receiver &operator=(const Receiver &) = delete;
    Receiver &operator=(Receiver &&) = delete;


    /** Stop listening after current association. */
    virtual void request_stop();

    /** Set AE Title. 
     *  @param ae_title AE Title to be used by the SCP.
     */ 
    void setaetitle(std::string ae_title);

    /** Set Port to listen on. 
     *  @param port Port to be used by the SCP. Default is "11112".
     */ 
    void setportnumber(Uint16 port);

    DcmDataset* getpooldataset();


    /** Set hostnames/IPs from which SCP can accept data. 
     *  @param source_list A list of strings of acceptable IPs/hostnames.
     */ 
    //void setacceptableIPs(OFList<OFString> source_list);

protected:
    /** Overwrite OFThread's run() method. */
    void run() override;
    //void setpooldataset(DcmDataset* dset);
};
 

#endif
