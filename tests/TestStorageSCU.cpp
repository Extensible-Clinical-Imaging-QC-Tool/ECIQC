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

  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

  // Define a separate transfer syntax needed for the X-ray image
  OFList<OFString> ts;
  ts.push_back(UID_LittleEndianImplicitTransferSyntax);

  addPresentationContext(UID_CTImageStorage, xfers);
  addPresentationContext(UID_MRImageStorage, xfers);
  addPresentationContext(UID_DigitalXRayImageStorageForPresentation, ts);
  addPresentationContext(UID_VerificationSOPClass, xfers);
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
