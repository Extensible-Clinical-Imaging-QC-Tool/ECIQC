#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */ 
#include <iostream>
#include <dcmtk/ofstd/ofstd.h> 
#include "dcmtk/ofstd/ofconapp.h"
#include <dcmtk/dcmnet/dstorscu.h> 
#include "dcmtk/dcmdata/cmdlnarg.h"  /* for prepareCmdLineArgs */
#include "Sender.hpp"


/* Sender Class 

Set parameters 
--------------

Takes in: 
1) host name of DICOM peer 
2) tcp/ip port number of peer 
3) DICOM file or directory to be transmitted

Processing of Input Files 
-------------------------
-Load input filenames, check if they are valid
-Create storage SCU and add the DICOM files to it 

Transfer
--------
-Add presentation context
-Initialise network
-Set association network with peer

--------
TO DO:
Log report, clean memory
--------
*/




/*
Used implementation of dcmsend.cc from:
https://github.com/InsightSoftwareConsortium/DCMTK/blob/master/dcmnet/apps/dcmsend.cc#L147
*/

#define OFFIS_CONSOLE_APPLICATION "SCU"

/* default application entity titles */
#define PEERAPPLICATIONTITLE "ANY-SCP"

/* host name of the peer machine */
#define PEERHOSTNAME "www.dicomserver.co.uk"

/* TCP/IP port to connect to the peer machine */
#define PEERPORT 11112 


/* Create Log */

/* exit codes for this command line tool */
#define EXITCODE_CANNOT_WRITE_REPORT_FILE        43
// network errors
#define EXITCODE_CANNOT_INITIALIZE_NETWORK       60
#define EXITCODE_CANNOT_NEGOTIATE_ASSOCIATION    61
#define EXITCODE_CANNOT_SEND_REQUEST             62
#define EXITCODE_CANNOT_ADD_PRESENTATION_CONTEXT 65


static OFLogger dcmsendLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

OFBool Sender::process(const char* opt_peer, const char* opt_peerTitle){


    OFOStringStream optStream;
    opt_peer = NULL;
    opt_peerTitle = PEERAPPLICATIONTITLE;
  

    OFCmdUnsignedInt opt_port = 0;
    OFBool opt_dicomDir = OFFalse;
    OFBool opt_haltOnInvalidFile = OFTrue;
    OFBool opt_multipleAssociations = OFTrue;

    /*command line parameters */
    OFCommandLine cmd;
   
    cmd.addParam("peer", "host name of DICOM peer");
    cmd.addParam("port", "tcp/ip port number of peer");
    cmd.addParam("dcmfile-in", "DICOM file or directory to be transmitted", OFCmdParam::PM_MultiMandatory);

    int paramCount = 0;
    paramCount = cmd.getParamCount();

    /* Iterate over input filenames */

    OFList<OFString> inputFiles; 
    const char *paramString = NULL;

    for (int i = 3; i <= paramCount; i++){
        cmd.getParam(i,paramString);
        if (OFStandard::dirExists(paramString)){
            OFStandard::searchDirectoryRecursively(paramString,inputFiles);
        }
        else{
            inputFiles.push_back(paramString);
        }
    }

    /* Check if there are input files */
    if (inputFiles.empty())
    {
        OFLOG_FATAL(dcmsendLogger,"input files not found");
        return EXITCODE_NO_INPUT_FILES;
    }

    //DcmStorageSCU SCU;
    DcmStorageSCU SCU; 
    OFCondition status;
    unsigned long numInvalidFiles = 0;


    /* set parameters used for processing the input files */
    SCU.setReadFromDICOMDIRMode(opt_dicomDir);
    SCU.setHaltOnInvalidFileMode(opt_haltOnInvalidFile);
    OFLOG_INFO(dcmsendLogger, "checking input files ...");


    /* iterate over all input filenames */
    const char *filename = NULL;

    OFListIterator(OFString) iter = inputFiles.begin();
    OFListIterator(OFString) last = inputFiles.end();

    if (status.good())
    {
        while (iter != last){
            filename = (*iter).c_str();
            status = SCU.addDicomFile(filename);
            if (status.bad())
            {
                /* check for empty filename */
                if (strlen(filename) == 0)
                    filename = "<empty string>";
                /* if something went wrong, we either terminate or ignore the file */
                if (opt_haltOnInvalidFile)
                {
                   OFLOG_FATAL(dcmsendLogger, "bad DICOM file: " << filename << ": " << status.text());
                //    cleanup();
                    return EXITCODE_INVALID_INPUT_FILE;
                } else {
                    OFLOG_WARN(dcmsendLogger, "bad DICOM file: " << filename << ": " << status.text() << ", ignoring file");
                }
                ++numInvalidFiles;
            }
            ++iter;
        }
    }


    /* check whether there are any valid input files */
    if (SCU.getNumberOfSOPInstances() == 0)
    {
        OFLOG_FATAL(dcmsendLogger, "no valid input files to be processed");
        return EXITCODE_NO_VALID_INPUT_FILES;

    } else {
        OFLOG_DEBUG(dcmsendLogger, "in total, there are " << SCU.getNumberOfSOPInstances()
            << " SOP instances to be sent, " << numInvalidFiles << " invalid files are ignored");
    }


    /*Prepare transfer */


    /* set network parameters */
    SCU.setPeerHostName(opt_peer);
    SCU.setPeerPort(OFstatic_cast(Uint16, opt_port));
    SCU.setPeerAETitle(opt_peerTitle);
    /* further parameters */
    //storageSCU.setMaxReceivePDULength(OFstatic_cast(Uint32, opt_maxReceivePDULength));
    //storageSCU.setACSETimeout(OFstatic_cast(Uint32, opt_acseTimeout));
    //storageSCU.setDIMSETimeout(OFstatic_cast(Uint32, opt_dimseTimeout));
    //storageSCU.setDIMSEBlockingMode(opt_blockMode);
    //storageSCU.setVerbosePCMode(opt_showPresentationContexts);


    /* output information on the single/multiple associations setting */
    if (opt_multipleAssociations)
    {
        OFLOG_DEBUG(dcmsendLogger, "multiple associations allowed (option --multi-associations used)");
    } else {
        OFLOG_DEBUG(dcmsendLogger, "only a single associations allowed (option --single-association used)");
    }

    // Use presentation context for FIND/MOVE in study root, propose all uncompressed transfer syntaxes 
    OFList<OFString> ts; 
    ts.push_back(UID_LittleEndianExplicitTransferSyntax); 
    ts.push_back(UID_BigEndianExplicitTransferSyntax); 
    ts.push_back(UID_LittleEndianImplicitTransferSyntax); 
    SCU.addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, ts); 
    SCU.addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, ts); 
    SCU.addPresentationContext(UID_VerificationSOPClass, ts); 

    /* Add presentation contexts to be negotiated */
    while ((status = SCU.addPresentationContexts()).good())
    {
        if (opt_multipleAssociations)
        {
            /* output information on the start of the new association */
            if (dcmsendLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
            {
                OFLOG_DEBUG(dcmsendLogger, OFString(65, '-') << OFendl
                    << "starting association #" << (SCU.getAssociationCounter() + 1));
            } else {
                OFLOG_INFO(dcmsendLogger, "starting association #" << (SCU.getAssociationCounter() + 1));
            }
        }
        OFLOG_INFO(dcmsendLogger, "initializing network ...");
        /* initialize network */
        status = SCU.initNetwork();
        if (status.bad())
        {
            OFLOG_FATAL(dcmsendLogger, "cannot initialize network: " << status.text());
            return EXITCODE_CANNOT_INITIALIZE_NETWORK;
        }
        OFLOG_INFO(dcmsendLogger, "negotiating network association ...");
        /* negotiate network association with peer */
        status = SCU.negotiateAssociation();
        if (status.bad())
        {
            // check whether we can continue with a new association
            if (status == NET_EC_NoAcceptablePresentationContexts)
            {
                OFLOG_ERROR(dcmsendLogger, "cannot negotiate network association: " << status.text());
                // check whether there are any SOP instances to be sent
                const size_t numToBeSent = SCU.getNumberOfSOPInstancesToBeSent();
                if (numToBeSent > 0)
                {
                    OFLOG_WARN(dcmsendLogger, "trying to continue with a new association "
                        << "in order to send the remaining " << numToBeSent << " SOP instances");
                }
            } else {
                OFLOG_FATAL(dcmsendLogger, "cannot negotiate network association: " << status.text());
                return EXITCODE_CANNOT_NEGOTIATE_ASSOCIATION;
            }
        }
        if (status.good())
        {
            OFLOG_INFO(dcmsendLogger, "sending SOP instances ...");
            /* send SOP instances to be transferred */
            status = SCU.sendSOPInstances();
            if (status.bad())
            {
                OFLOG_FATAL(dcmsendLogger, "cannot send SOP instance: " << status.text());
                // handle certain error conditions (initiated by the communication peer)
                if (status == DUL_PEERREQUESTEDRELEASE)
                {
                    // peer requested release (aborting)
                    SCU.closeAssociation(DCMSCU_PEER_REQUESTED_RELEASE);
                }
                else if (status == DUL_PEERABORTEDASSOCIATION)
                {
                    // peer aborted the association
                    SCU.closeAssociation(DCMSCU_PEER_ABORTED_ASSOCIATION);
                }
                
                return EXITCODE_CANNOT_SEND_REQUEST;
            }
        }
        /* close current network association */
        SCU.releaseAssociation();
        /* check whether multiple associations are permitted */
        if (!opt_multipleAssociations)
            break;
    }



    /*Create log report */
    const char *report_filename = "Sender_Logs";
    status = SCU.createReportFile(report_filename);
    if (status.bad()){
        //throw "Cannot write to output file";
        return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
    }

}
