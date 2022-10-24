#include "TestStorageSCU.hpp"

TestStorageSCU::TestStorageSCU(std::string title, int port)
    : m_title(std::move(title)), m_peer_aetitle("SCP"), m_hostname("localhost"),
      m_port(port) {}

void TestStorageSCU::initialise() {
  setAETitle(m_title.c_str());
  setPeerAETitle(m_peer_aetitle.c_str());
  setPeerHostName(m_hostname.c_str());
  setPeerPort(m_port);
  setVerbosePCMode(OFTrue);

  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
  addPresentationContext(UID_CTImageStorage, xfers);
  addPresentationContext(UID_MRImageStorage, xfers);

  // Define a separate transfer syntax needed for the X-ray image
  OFList<OFString> xfer_xray;
  xfer_xray.push_back(UID_LittleEndianImplicitTransferSyntax);
  addPresentationContext(UID_DigitalXRayImageStorageForPresentation, xfer_xray);

  addPresentationContext(UID_VerificationSOPClass, xfers);
  initNetwork();
}

void TestStorageSCU::run() {
  Uint16 rspStatusCode = 0;
  negotiateAssociation();
  for (int i = 0; i < m_filenames.size(); i++) {
    m_results[i] =
        sendSTORERequest(0, m_filenames[i].c_str(), nullptr, rspStatusCode);
  }
  releaseAssociation();
}
