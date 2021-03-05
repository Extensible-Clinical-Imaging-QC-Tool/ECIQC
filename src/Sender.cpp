/* 
 * 
 *  Copyright (C) 2011-2012, OFFIS e.V. 
 *  All rights reserved.  See COPYRIGHT file for details. 
 * 
 *  This software and supporting documentation were developed by 
 * 
 *    OFFIS e.V. 
 *    R&D Division Health 
 *    Escherweg 2 
 *    D-26121 Oldenburg, Germany 
 * 
 * 
 *  Module:  dcmnet 
 * 
 *  Author:  Michael Onken 
 * 
 *  Purpose: Test for move feature of the DcmSCU class 
 */ 

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */ 
#include "dcmtk/dcmnet/scu.h" 
#include "dcmtk/dcmnet/diutil.h" 
#include "Sender.hpp"


#define OFFIS_CONSOLE_APPLICATION "testscu" 

static OFLogger echoscuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION); 

//static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v" 
//  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $"; 

// our application entity title used for calling the peer machine 
#define APPLICATIONTITLE     "TEST-SCU" 

// host name of the peer machine 
#define PEERHOSTNAME         "www.dicomserver.co.uk" 

// TCP/IP port to connect to the peer machine 
#define PEERPORT 11112 

// application entity title of the peer machine 
#define PEERAPPLICATIONTITLE "MOVESCP" 

// MOVE destination AE Title 
#define MOVEAPPLICATIONTITLE "TEST-SCU" 

static Uint8 findUncompressedPC(const OFString& sopClass, 
                                DcmSCU& scu) 
{ 
  Uint8 pc; 
  pc = scu.findPresentationContextID(sopClass, UID_LittleEndianExplicitTransferSyntax); 
  if (pc == 0) 
    pc = scu.findPresentationContextID(sopClass, UID_BigEndianExplicitTransferSyntax); 
  if (pc == 0) 
    pc = scu.findPresentationContextID(sopClass, UID_LittleEndianImplicitTransferSyntax); 
  return pc; 
} 
// ----------------------------------------------------------------------------

Sender::~Sender() {}

// ----------------------------------------------------------------------------

OFCondition Sender::handleOutcomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo)
    {

        if (incomingMsg->CommandField == DIMSE_C_STORE_RQ)
        {
            // Enable handling of C-FIND response.
            
            DcmDataset* dset = NULL;
            return DcmSCU::handleFINDResponse(incomingMsg->msg.CStoreRQ, presInfo.presentationContextID, dset);
        }

        else
        {
            return DcmSCU::handleOutcomingCommand(incomingMsg, presInfo);
        }
    }

// ----------------------------------------------------------------------------

OFBool Sender::checkCallingHostAccepted(const OFString& hostOrIP)
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

OFBool Sender::checkCallingAETitleAccepted(const OFString& callingAE)
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

OFCondition Sender::setIPs(const OFList<OFString>& source_list)
{
    if (isConnected())
  {
    return EC_IllegalCall; // TODO: need to find better error code
  }
  m_sourcelist = source_list;
  return EC_Normal;
}

// ----------------------------------------------------------------------------

OFCondition Sender::setpeerAETitles(const OFList<OFString>& peerae_list){
     if (isConnected())
  {
    return EC_IllegalCall; // TODO: need to find better error code
  }
  m_peerAETitles = peerae_list;
  return EC_Normal;
}

// **  SENDER  

Sender::Sender() { 

  /* Setup DICOM connection parameters */ 
  OFLog::configure(OFLogger::DEBUG_LOG_LEVEL); 
  DcmSCU scu; 
  // set AE titles 
  scu.setAETitle(APPLICATIONTITLE); 
  scu.setPeerHostName(PEERHOSTNAME); 
  scu.setPeerPort(PEERPORT); 
  scu.setPeerAETitle(PEERAPPLICATIONTITLE); 
  // Use presentation context for FIND/MOVE in study root, propose all uncompressed transfer syntaxes 
  OFList<OFString> ts; 
  ts.push_back(UID_LittleEndianExplicitTransferSyntax); 
  ts.push_back(UID_BigEndianExplicitTransferSyntax); 
  ts.push_back(UID_LittleEndianImplicitTransferSyntax); 
  scu.addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, ts); 
  scu.addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, ts); 
  scu.addPresentationContext(UID_VerificationSOPClass, ts); 

  
}
