#ifndef SENDER_H 
#define SENDER_H 

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */ 

#include "dcmtk/dcmnet/dstorscu.h"     /* Covers most common dcmdata classes */ 

class Sender : public DcmStorageSCU 
{ 

public: 

  OFCondition result;
  OFCondition status;
  //status = addDicomFile("../DICOM_Images/1-1copy.dcm", ERM_fileOnly, false);

 /**Constructor. */
  Sender(std::string ae_title, std::string peer_hostname, Uint16 peer_port, std::string peer_aetitles);

  OFCondition send(DcmDataset& dataset);

};

 



 

#endif // TESTSCU_H 
