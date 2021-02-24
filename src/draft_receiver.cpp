#include <iostream>
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmpstat/dcmpstat.h>

#include "draft_receiver.hpp"


const char *ReceiverThread::DEF_StandardSubdirectory  = "data";
const char *ReceiverThread::DEF_UndefinedSubdirectory = "undef";
const char *ReceiverThread::DEF_FilenameExtension     = "";

// ----------------------------------------------------------------------------

ReceiverThread::ReceiverThread()
    : DcmThreadSCP(),
      DatasetStorage(DSM_Default),
      DirectoryGeneration(DGM_Default),
      FilenameGeneration(FGM_Default),
      StandardSubdirectory(DEF_StandardSubdirectory),
      UndefinedSubdirectory(DEF_UndefinedSubdirectory),
      FilenameExtension(DEF_FilenameExtension)
{

}

// ----------------------------------------------------------------------------

ReceiverThread::~ReceiverThread() {}

// ----------------------------------------------------------------------------

OFCondition ReceiverThread::handleIncomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo)
    {
        OFCondition status = EC_IllegalParameter;
        if (incomingMsg->CommandField == DIMSE_C_STORE_RQ)
        {
            // Enable handling of C-STORE requests.
            // handle incoming C-STORE request
            T_DIMSE_C_StoreRQ &storeReq = incomingMsg->msg.CStoreRQ;
            Uint16 rspStatusCode = STATUS_STORE_Error_CannotUnderstand;

            // special case: bit preserving mode
            if (DatasetStorage == DGM_StoreBitPreserving)
            {
                OFString filename;
                // generate filename with full path (and create subdirectories if needed)
                status = generateSTORERequestFilename(storeReq, filename);
                if (status.good())
                {
                    if (OFStandard::fileExists(filename))
                        DCMNET_WARN("file already exists, overwriting: " << filename);
                    // receive dataset directly to file
                    status = receiveSTORERequest(storeReq, presInfo.presentationContextID, filename);
                    if (status.good())
                    {
                        // call the notification handler (default implementation outputs to the logger)
                        notifyInstanceStored(filename, storeReq.AffectedSOPClassUID, storeReq.AffectedSOPInstanceUID);
                        rspStatusCode = STATUS_Success;
                    }
                }
            } else {
                DcmFileFormat fileformat;
                DcmDataset *reqDataset = fileformat.getDataset();
                // receive dataset in memory
                status = receiveSTORERequest(storeReq, presInfo.presentationContextID, reqDataset);

                if (status.good())
                {
                // do we need to store the received dataset at all?
                if (DatasetStorage == DSM_Ignore)
                    {
                    // output debug message that dataset is not stored
                    DCMNET_DEBUG("received dataset is not stored since the storage mode is set to 'ignore'");
                    rspStatusCode = STATUS_Success;
                    } else {
                    // check and process C-STORE request
                    rspStatusCode = checkAndProcessSTORERequest(storeReq, fileformat);
                    }
                }

            }

            
            // send C-STORE response (with DIMSE status code)
            if (status.good())
                status = sendSTOREResponse(presInfo.presentationContextID, storeReq, rspStatusCode);
            else if (status == DIMSE_OUTOFRESOURCES)
            {
                // do not overwrite the previous error status
                sendSTOREResponse(presInfo.presentationContextID, storeReq, STATUS_STORE_Refused_OutOfResources);
            }
            
            return status;
            //DcmDataset* dset = NULL;
            //return DcmSCP::handleSTORERequest(incomingMsg->msg.CStoreRQ, presInfo.presentationContextID, dset);
        }

        else
        {
            return DcmSCP::handleIncomingCommand(incomingMsg, presInfo);
        }
    }

// ----------------------------------------------------------------------------
Uint16 ReceiverThread::checkAndProcessSTORERequest(const T_DIMSE_C_StoreRQ &reqMessage,
                                                  DcmFileFormat &fileformat)
{
    DCMNET_DEBUG("checking and processing C-STORE request");
    Uint16 statusCode = STATUS_STORE_Error_CannotUnderstand;
    DcmDataset *dataset = fileformat.getDataset();
    // perform some basic checks on the request dataset
    if ((dataset != NULL) && !dataset->isEmpty())
    {
        OFString filename;
        OFString directoryName;
        OFString sopClassUID = reqMessage.AffectedSOPClassUID;
        OFString sopInstanceUID = reqMessage.AffectedSOPInstanceUID;
        // generate filename with full path
        OFCondition status = generateDirAndFilename(filename, directoryName, sopClassUID, sopInstanceUID, dataset);
        if (status.good())
        {
            DCMNET_DEBUG("generated filename for received object: " << filename);
            // create the output directory (if needed)
            status = OFStandard::createDirectory(directoryName, OutputDirectory /* rootDir */);
            if (status.good())
            {
                if (OFStandard::fileExists(filename))
                    DCMNET_WARN("file already exists, overwriting: " << filename);
                // store the received dataset to file (with default settings)
                status = fileformat.saveFile(filename);
                if (status.good())
                {
                    // call the notification handler (default implementation outputs to the logger)
                    notifyInstanceStored(filename, sopClassUID, sopInstanceUID, dataset);
                    statusCode = STATUS_Success;
                } else {
                    DCMNET_ERROR("cannot store received object: " << filename << ": " << status.text());
                    statusCode = STATUS_STORE_Refused_OutOfResources;

                    // delete incomplete file
                    OFStandard::deleteFile(filename);
                }
            } else {
                DCMNET_ERROR("cannot create directory for received object: " << directoryName << ": " << status.text());
                statusCode = STATUS_STORE_Refused_OutOfResources;
            }
        } else
            DCMNET_ERROR("cannot generate directory or file name for received object: " << status.text());
    }
    return statusCode;
}

// ----------------------------------------------------------------------------

void ReceiverThread::notifyInstanceStored(const OFString &filename,
                                         const OFString & /*sopClassUID*/,
                                         const OFString & /*sopInstanceUID*/,
                                         DcmDataset * /*dataset*/) const
{
    // by default, output some useful information
    DCMNET_INFO("Stored received object to file: " << filename);
}
// ----------------------------------------------------------------------------
OFCondition ReceiverThread::generateSTORERequestFilename(const T_DIMSE_C_StoreRQ &reqMessage,
                                                        OFString &filename)
{
    OFString directoryName;
    OFString sopClassUID = reqMessage.AffectedSOPClassUID;
    OFString sopInstanceUID = reqMessage.AffectedSOPInstanceUID;
    // generate filename (with full path)
    OFCondition status = generateDirAndFilename(filename, directoryName, sopClassUID, sopInstanceUID);
    if (status.good())
    {
        DCMNET_DEBUG("generated filename for object to be received: " << filename);
        // create the output directory (if needed)
        status = OFStandard::createDirectory(directoryName, OutputDirectory /* rootDir */);
        if (status.bad())
            DCMNET_ERROR("cannot create directory for object to be received: " << directoryName << ": " << status.text());
    } else
        DCMNET_ERROR("cannot generate directory or file name for object to be received: " << status.text());
    return status;
}
// ----------------------------------------------------------------------------
OFCondition ReceiverThread::generateDirAndFilename(OFString &filename,
                                                  OFString &directoryName,
                                                  OFString &sopClassUID,
                                                  OFString &sopInstanceUID,
                                                  DcmDataset *dataset)
{
    OFCondition status = EC_Normal;
    // get SOP class and instance UID (if not yet known from the command set)
    if (dataset != NULL)
    {
        if (sopClassUID.empty())
            dataset->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
        if (sopInstanceUID.empty())
            dataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
    }
    // generate directory name
    OFString generatedDirName;
    switch (DirectoryGeneration)
    {
        case DGM_NoSubdirectory:
            // do nothing (default)
            break;
        // use series date (if available) for subdirectory structure
        case DGM_SeriesDate:
            if (dataset != NULL)
            {
                OFString seriesDate;
                DcmElement *element = NULL;
                // try to get the series date from the dataset
                if (dataset->findAndGetElement(DCM_SeriesDate, element).good() && (element->ident() == EVR_DA))
                {
                    OFString dateValue;
                    DcmDate *dateElement = OFstatic_cast(DcmDate *, element);
                    // output ISO format is: YYYY-MM-DD
                    if (dateElement->getISOFormattedDate(dateValue).good() && (dateValue.length() == 10))
                    {
                        OFOStringStream stream;
                        stream << StandardSubdirectory << PATH_SEPARATOR
                            << dateValue.substr(0, 4) << PATH_SEPARATOR
                            << dateValue.substr(5 ,2) << PATH_SEPARATOR
                            << dateValue.substr(8, 2) << OFStringStream_ends;
                        OFSTRINGSTREAM_GETSTR(stream, tmpString)
                        generatedDirName = tmpString;
                        OFSTRINGSTREAM_FREESTR(tmpString);
                    }
                }
                // alternatively, if that fails, use the current system date
                if (generatedDirName.empty())
                {
                    OFString currentDate;
                    status = DcmDate::getCurrentDate(currentDate);
                    if (status.good())
                    {
                        OFOStringStream stream;
                        stream << UndefinedSubdirectory << PATH_SEPARATOR
                            << currentDate << OFStringStream_ends;
                        OFSTRINGSTREAM_GETSTR(stream, tmpString)
                        generatedDirName = tmpString;
                        OFSTRINGSTREAM_FREESTR(tmpString);
                    }
                }
            } else {
                DCMNET_DEBUG("received dataset is not available in order to determine the SeriesDate "
                    << DCM_SeriesDate << ", are you using the bit preserving mode?");
                // no DICOM dataset given, so we cannot determine the series date
                status = EC_CouldNotGenerateDirectoryName;
            }
            break;
    }
    if (status.good())
    {
        // combine the generated directory name with the output directory
        OFStandard::combineDirAndFilename(directoryName, OutputDirectory, generatedDirName);
        // generate filename
        OFString generatedFileName;
        switch (FilenameGeneration)
        {
            // use modality prefix and SOP instance UID (default)
            case FGM_SOPInstanceUID:
            {
                if (sopClassUID.empty())
                    status = NET_EC_InvalidSOPClassUID;
                else if (sopInstanceUID.empty())
                    status = NET_EC_InvalidSOPInstanceUID;
                else {
                    OFOStringStream stream;
                    stream << dcmSOPClassUIDToModality(sopClassUID.c_str(), "UNKNOWN")
                           << '.' << sopInstanceUID << FilenameExtension << OFStringStream_ends;
                    OFSTRINGSTREAM_GETSTR(stream, tmpString)
                    generatedFileName = tmpString;
                    OFSTRINGSTREAM_FREESTR(tmpString);
                    // combine the generated file name with the directory name
                    OFStandard::combineDirAndFilename(filename, directoryName, generatedFileName);
                }
                break;
            }
            // unique filename based on modality prefix and newly generated UID
            case FGM_UniqueFromNewUID:
            {
                char uidBuffer[70];
                dcmGenerateUniqueIdentifier(uidBuffer);
                OFOStringStream stream;
                stream << dcmSOPClassUIDToModality(sopClassUID.c_str(), "UNKNOWN")
                       << ".X." << uidBuffer << FilenameExtension << OFStringStream_ends;
                OFSTRINGSTREAM_GETSTR(stream, tmpString)
                generatedFileName = tmpString;
                OFSTRINGSTREAM_FREESTR(tmpString);
                // combine the generated file name with the directory name
                OFStandard::combineDirAndFilename(filename, directoryName, generatedFileName);
                break;
            }
            // unique pseudo-random filename (also checks for existing files, so we need some special handling)
            case FGM_ShortUniquePseudoRandom:
            {
                OFString prefix = dcmSOPClassUIDToModality(sopClassUID.c_str(), "XX");
                prefix += '_';
                // TODO: we might want to use a more appropriate seed value
                unsigned int seed = OFstatic_cast(unsigned int, time(NULL));
                if (!FilenameCreator.makeFilename(seed, directoryName.c_str(), prefix.c_str(), FilenameExtension.c_str(), filename))
                    status = EC_CouldNotGenerateFilename;
                break;
            }
            // use current system time and modality suffix for filename
            case FGM_CurrentSystemTime:
            {
                OFString timeStamp;
                // get the date/time as: YYYYMMDDHHMMSS.FFFFFF
                if (DcmDateTime::getCurrentDateTime(timeStamp, OFTrue /* seconds */, OFTrue /* fraction */).good())
                {
                    OFOStringStream stream;
                    stream << timeStamp << '.' << dcmSOPClassUIDToModality(sopClassUID.c_str(), "UNKNOWN")
                        << FilenameExtension << OFStringStream_ends;
                    OFSTRINGSTREAM_GETSTR(stream, tmpString)
                    generatedFileName = tmpString;
                    OFSTRINGSTREAM_FREESTR(tmpString);
                    // combine the generated file name
                    OFStandard::combineDirAndFilename(filename, directoryName, generatedFileName);
                } else
                    status = EC_CouldNotGenerateFilename;
                break;
            }

        }
    }
    return status;
}

// ----------------------------------------------------------------------------

OFBool ReceiverThread::checkCallingHostAccepted(const OFString& hostOrIP)
{
    // Check if acceptable IPs/hostnames have been specified. 
    if(m_sourcelist.size()!= 0) {
        
        // Check if peer's hostname is in the acceptable source list.
        OFListIterator(OFString) it = m_sourcelist.begin();
        OFListIterator(OFString) last = m_sourcelist.end();
        
        while(it != last)
        {
            OFString item = *it;
            if (item == getPeerIP())
            {
                return OFTrue;
                it = last;
            }
            ++it;
        }
        return OFFalse;
    }
    else
    {
        return OFTrue;
    }
    

}

// ----------------------------------------------------------------------------

OFBool ReceiverThread::checkCallingAETitleAccepted(const OFString& callingAE)
{
    if(m_peerAETitles.size() != 0)
    {
        OFListIterator(OFString) it = m_peerAETitles.begin();
        OFListIterator(OFString) last = m_peerAETitles.end();

        while(it != last)
        {
            OFString item = *it;
            if (item == getPeerAETitle())
            {
                return OFTrue;
                it = last;
            }
            ++it;
        }
        
        return OFFalse;
    }
    else
    {
        return OFTrue;
    }
}

// ----------------------------------------------------------------------------

OFCondition ReceiverThread::setIPs(const OFList<OFString>& source_list)
{
    if (isConnected())
  {
    return EC_IllegalCall; // TODO: need to find better error code
  }
  m_sourcelist = source_list;
  return EC_Normal;
}

// ----------------------------------------------------------------------------

OFCondition ReceiverThread::setpeerAETitles(const OFList<OFString>& peerae_list){
     if (isConnected())
  {
    return EC_IllegalCall; // TODO: need to find better error code
  }
  m_peerAETitles = peerae_list;
  return EC_Normal;
}

// ----------------------------------------------------------------------------
OFCondition ReceiverThread::setOutputDirectory(const OFString &directory)
{
    OFCondition status = EC_Normal;
    if (directory.empty())
    {
        // empty directory refers to the current directory
        if (OFStandard::isWriteable("."))
            OutputDirectory.clear();
        else
            status = EC_DirectoryNotWritable;
    } else {
        // check whether given directory exists and is writable
        if (OFStandard::dirExists(directory))
        {
            if (OFStandard::isWriteable(directory))
                OFStandard::normalizeDirName(OutputDirectory, directory);
            else
                status = EC_DirectoryNotWritable;
        } else
            status = EC_DirectoryDoesNotExist;
    }
    return status;
}
// ----------------------------------------------------------------------------
void ReceiverThread::setDirectoryGenerationMode(const E_DirectoryGenerationMode mode)
{
    DirectoryGeneration = mode;
}

void ReceiverThread::setFilenameGenerationMode(const E_FilenameGenerationMode mode)
{
    FilenameGeneration = mode;
}


void ReceiverThread::setFilenameExtension(const OFString &extension)
{
    FilenameExtension = extension;
}

// ----------------------------------------------------------------------------
void ReceiverThread::setDatasetStorageMode(const E_DatasetStorageMode mode)
{
    DatasetStorage = mode;
}

/* *********************************************************************** */
/*                        Receiver class                                   */
/* *********************************************************************** */

Receiver::Receiver()
{
        // Configure SCP port
        getConfig().setPort(11112);
        // Configure SCP name
        getConfig().setAETitle("TestSCP");
        // Set number of threads
        setMaxThreads(2);
        getConfig().setConnectionBlockingMode(DUL_NOBLOCK);

        // Set time to wait for the next association (in seconds)
        getConfig().setConnectionTimeout(5);
        // Set verbose mode
        getConfig().setVerbosePCMode(OFTrue);

        // Add presentation context to be handled
        OFList<OFString> ts;
        ts.push_back(UID_LittleEndianExplicitTransferSyntax);
        ts.push_back(UID_LittleEndianImplicitTransferSyntax);
        ts.push_back(UID_BigEndianExplicitTransferSyntax);
        ts.push_back(UID_JPEGProcess14SV1TransferSyntax);
        ts.push_back(UID_JPEGProcess1TransferSyntax);
        for (size_t n = 0; n < numberOfDcmLongSCUStorageSOPClassUIDs; n++)
        {
            getConfig().addPresentationContext(dcmLongSCUStorageSOPClassUIDs[n], ts);
        }
        getConfig().addPresentationContext(UID_VerificationSOPClass, ts);
    }

// ----------------------------------------------------------------------------

Receiver::~Receiver() {}

// ----------------------------------------------------------------------------

void Receiver::request_stop(){ DQDBaseSCPPool::stopAfterCurrentAssociations(); }

// ----------------------------------------------------------------------------

void Receiver::run()
    {
        result = DQDSCPPool :: listen();
    }

// ----------------------------------------------------------------------------

void Receiver::setaetitle(OFString ae_title) 
{
    getConfig().setAETitle(ae_title);
}

// ----------------------------------------------------------------------------

void Receiver::setportnumber(Uint16 port)
{
    getConfig().setPort(port);
}


    