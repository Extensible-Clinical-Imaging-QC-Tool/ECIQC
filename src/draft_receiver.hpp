#ifndef RECEIVER_H
#define RECEIVER_H

#include <dcmtk/dcmnet/scppool.h>
#include <dcmtk/dcmnet/scpthrd.h>
#include <dcmtk/ofstd/offname.h>   /* for OFFilenameCreator */
#include "poolbase.h"
/**
 * A worker thread in a multithreaded Service Class Provider. 
 * Runs an association from an already accepted connection.
 */
class ReceiverThread : public DcmThreadSCP
{ OFList<OFString> m_sourcelist;
  OFList<OFString> m_peerAETitles;

public:

    
    /** modes for generating subdirectories
     */
    enum E_DirectoryGenerationMode
    {
        /// do not generate any subdirectories
        DGM_NoSubdirectory,
        /// generated subdirectories based on the Series Date (0008,0021)
        DGM_SeriesDate,
        /// default value
        DGM_Default = DGM_NoSubdirectory
    };

    /** modes for generating filenames
     */
    enum E_FilenameGenerationMode
    {
        /// generate filename from SOP Instance UID (0008,0018)
        FGM_SOPInstanceUID,
        /// generate unique filename based on new UID
        FGM_UniqueFromNewUID,
        /// generate short pseudo-random unique filename
        FGM_ShortUniquePseudoRandom,
        /// generate filename from current system time
        FGM_CurrentSystemTime,
        /// default value
        FGM_Default = FGM_SOPInstanceUID
    };

    /** modes specifying whether and how to store the received datasets
     */
    enum E_DatasetStorageMode
    {
        /// receive dataset in memory, perform some conversions and store it to file
        DGM_StoreToFile,
        /// receive dataset directly to file, i.e. write data exactly as received
        DGM_StoreBitPreserving,
        /// receive dataset in memory, but do not store it to file
        DSM_Ignore,
        /// default value
        DSM_Default = DSM_Ignore
    };
    /**  Constructor. */
    ReceiverThread();

    /** Destructor. */
    virtual ~ReceiverThread();

    /** Overwrite method of DcmSCP to enable handling of C-STORE requests. */
    OFCondition handleIncomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo);

    Uint16 checkAndProcessSTORERequest(const T_DIMSE_C_StoreRQ &reqMessage, DcmFileFormat &fileformat);

    OFCondition generateDirAndFilename(OFString &filename, OFString &directoryName, OFString &sopClassUID, OFString &sopInstanceUID, DcmDataset *dataset = NULL);

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
    virtual OFBool checkCallingHostAccepted(const OFString& hostOrIP);

    /** Check if calling AE Title is accepted by the SCP.
     *  @param callingAE Connected peer's AE Title.
     *  @return OFTrue if calling AE Title is allowed (or if no acceptable calling AE Titles are specified).
     *          OFFalse if calling AE is not accepted.
     */
    virtual OFBool checkCallingAETitleAccepted(const OFString& callingAE); 

    void setDatasetStorageMode(const E_DatasetStorageMode mode);
    
    virtual void notifyInstanceStored(const OFString &filename,
                                      const OFString &sopClassUID,
                                      const OFString &sopInstanceUID,
                                      DcmDataset *dataset = NULL) const;

    // --- public constants ---

    /// default value for the name of the subdirectory that might be used for the
    /// "normal" case
    static const char *DEF_StandardSubdirectory;
    /// default value for the name of the subdirectory that might be used for the
    /// "exceptional" case
    static const char *DEF_UndefinedSubdirectory;
    /// default value for the filename extension appended to the generated filenames
    static const char *DEF_FilenameExtension;

private:
    
    /// mode specifying how to store the received datasets (also allows for skipping the storage)
    E_DatasetStorageMode DatasetStorage;
    /// mode that is used to generate subdirectories to store the received datasets
    E_DirectoryGenerationMode DirectoryGeneration;
    /// mode that is used to generate filenames for the received datasets
    E_FilenameGenerationMode FilenameGeneration;
    /// name of the subdirectory that might be used for the "normal" case, i.e.\ if the
    /// name of the subdirectory could be generated according to the current mode
    OFString StandardSubdirectory;
    /// name of the subdirectory that might be used for the "exceptional" case, i.e.\ if
    /// the name of the subdirectory could not be generated according to the current mode
    OFString UndefinedSubdirectory;
    /// filename extension appended to the generated filenames
    OFString FilenameExtension;

    /// name of the output directory that is used to store the received datasets
    OFString OutputDirectory;
    /// unique pseudo-random filename creator, which also checks for existing files
    OFFilenameCreator FilenameCreator;

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
    void setaetitle(OFString ae_title);

    /** Set Port to listen on. 
     *  @param port Port to be used by the SCP. Default is "11112".
     */ 
    void setportnumber(Uint16 port);

    /** Set hostnames/IPs from which SCP can accept data. 
     *  @param source_list A list of strings of acceptable IPs/hostnames.
     */ 
    //void setacceptableIPs(OFList<OFString> source_list);

protected:
    /** Overwrite OFThread's run() method. */
    void run();
};
 

#endif