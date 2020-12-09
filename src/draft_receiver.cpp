#include <iostream>
#include "draft_receiver.hpp"

// constructor
ReceiverThread::ReceiverThread():DcmThreadSCP()
{

};

// destructor
ReceiverThread::~ReceiverThread() {};

OFCondition ReceiverThread::handleIncomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo)
    {

        if (incomingMsg->CommandField == DIMSE_C_STORE_RQ)
        {
            // handle incoming C-STORE request
            
            DcmDataset* dset = NULL;
            return DcmSCP::handleSTORERequest(incomingMsg->msg.CStoreRQ, presInfo.presentationContextID, dset);
        }
        /* 
        // save to file code
+            DcmFileFormat fileformat;
+            DcmDataset* reqDataset = fileformat.getDataset();
+            OFCondition result = DcmSCP::receiveSTORERequest(incomingMsg->msg.CStoreRQ, presInfo.presentationContextID, reqDataset);
+                   if (result.good())
+                   {
+                       result = fileformat.saveFile("/home/sabsr3/filetest.dcm");
+                   }
+            return result;

        */
        else
        {
            return DcmSCP::handleIncomingCommand(incomingMsg, presInfo);
        }
    };

// constructor part 2
Receiver::Receiver()
{
        // Configure SCP, only port is required
        getConfig().setPort(11112);
        getConfig().setAETitle("TestSCP");
        //getConfig().setRespondWithCalledAETitle(OFTrue);
        setMaxThreads(2);
        getConfig().setConnectionBlockingMode(DUL_NOBLOCK);
        getConfig().setConnectionTimeout(20);
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
        getConfig().addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts);
    };

Receiver::~Receiver() {};

void Receiver::request_stop(){ DcmBaseSCPPool::stopAfterCurrentAssociations(); };

void Receiver::run(){
        result = DcmSCPPool :: listen();
    };
