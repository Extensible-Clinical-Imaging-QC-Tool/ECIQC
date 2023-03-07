#include "TestStorageSCU.hpp"
#include <cstddef>

TestStorageSCU::TestStorageSCU()
    : m_title("TestSCU"), m_peer_aetitle("TestSCP"),
      m_peer_hostname("localhost"), m_peer_port(11112) {}

TestStorageSCU::TestStorageSCU(std::string title, std::string peer_title,
                               int port)
    : m_title(std::move(title)), m_peer_aetitle(std::move(peer_title)),
      m_peer_hostname("localhost"), m_peer_port(port) {}

void TestStorageSCU::initialise() {
  setAETitle(m_title.c_str());
  setPeerAETitle(m_peer_aetitle.c_str());
  setPeerHostName(m_peer_hostname.c_str());
  setPeerPort(m_peer_port);
  setVerbosePCMode(OFTrue);

  // Add presentation context to be handled
  // Tried by Yiming 2023-03-03 uncomment codes from line 22 to line 42 if going back!
  /*
  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
  //xfers.push_back(UID_JPEGProcess14SV1TransferSyntax);
  //xfers.push_back(UID_JPEGProcess1TransferSyntax);

  // Define a separate transfer syntax needed for the X-ray image
  OFList<OFString> ts;
  ts.push_back(UID_LittleEndianImplicitTransferSyntax);
  //ts.push_back(UID_JPEGProcess14SV1TransferSyntax);
  //ts.push_back(UID_JPEGProcess1TransferSyntax);

  addPresentationContext(UID_CTImageStorage, xfers);
  addPresentationContext(UID_MRImageStorage, xfers);
  //addPresentationContext(UID_UltrasoundMultiframeImageStorage,ts);
  //addPresentationContext(UID_UltrasoundMultiframeImageStorage,xfers);
  //addPresentationContext(UID_SecondaryCaptureImageStorage,ts);
  //addPresentationContext(UID_SecondaryCaptureImageStorage,xfers);
  
  addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts);
  addPresentationContext(UID_VerificationSOPClass, xfers);
  */

  /*
  OFList<OFString> ts1;
  OFList<OFString> ts2;
  ts1.push_back(UID_LittleEndianExplicitTransferSyntax);
  ts1.push_back(UID_LittleEndianImplicitTransferSyntax);
  ts2.push_back(UID_JPEGProcess1TransferSyntax);

  addPresentationContext(UID_CTImageStorage, ts1);
  addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts1);
  addPresentationContext(UID_SecondaryCaptureImageStorage,ts1);
  addPresentationContext(UID_MRImageStorage, ts1);
  addPresentationContext(UID_VerificationSOPClass, ts1);
  addPresentationContext(UID_UltrasoundMultiframeImageStorage,ts2);
  */
  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
  //xfers.push_back(UID_JPEGProcess14SV1TransferSyntax);
  //xfers.push_back(UID_JPEGProcess1TransferSyntax);

  // Define a separate transfer syntax needed for the X-ray image
  OFList<OFString> ts;
  ts.push_back(UID_LittleEndianImplicitTransferSyntax);
  //ts.push_back(UID_JPEGProcess14SV1TransferSyntax);
  //ts.push_back(UID_JPEGProcess1TransferSyntax);

  OFList<OFString> ts2;
  ts2.push_back(UID_JPEGProcess1TransferSyntax);

  addPresentationContext(UID_CTImageStorage, xfers);
  addPresentationContext(UID_MRImageStorage, xfers);
  addPresentationContext(UID_SecondaryCaptureImageStorage,xfers);
  addPresentationContext(UID_VerificationSOPClass, xfers);
  addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts);
  addPresentationContext(UID_UltrasoundMultiframeImageStorage,ts2);
  initNetwork();
}

void TestStorageSCU::run() {
  Uint16 rspStatusCode = 0;
  m_negotiate_association = negotiateAssociation();
  m_echo_result = sendECHORequest(0);
  for (size_t i = 0; i < m_filenames.size(); i++) {
    m_results[i] =
        sendSTORERequest(0, m_filenames[i].c_str(), nullptr, rspStatusCode);
  }
  m_release_association = releaseAssociation();
}
