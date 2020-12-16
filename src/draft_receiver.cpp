#include <iostream>
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmpstat/dcmpstat.h>

#include "draft_receiver.hpp"


ReceiverThread::ReceiverThread():DcmThreadSCP()
{

}

ReceiverThread::~ReceiverThread() {}

OFCondition ReceiverThread::handleIncomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo)
    {

        if (incomingMsg->CommandField == DIMSE_C_STORE_RQ)
        {
            // Enable handling of C-STORE requests.
            
            DcmDataset* dset = NULL;
            return DcmSCP::handleSTORERequest(incomingMsg->msg.CStoreRQ, presInfo.presentationContextID, dset);
        }

        else
        {
            return DcmSCP::handleIncomingCommand(incomingMsg, presInfo);
        }
    }

/*OFBool ReceiverThread::checkCallingHostAccepted(){

} */

void ReceiverThread::setIPs(OFList<OFString> source_list){
    m_sourcelist = source_list;
}

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
        getConfig().setConnectionTimeout(20);
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

void Receiver::request_stop(){ DQDBaseSCPPool::stopAfterCurrentAssociations(); }

void Receiver::run(){
        result = DQDSCPPool :: listen();
    }

void Receiver::set_name(OFString ae_title) {
    getConfig().setAETitle(ae_title);
}

void Receiver::setacceptableIPs(OFList<OFString> source_list){
    m_sourcelist = source_list;
}

OFList<OFString> Receiver::getacceptableIPs(){
    return m_sourcelist;
}
    