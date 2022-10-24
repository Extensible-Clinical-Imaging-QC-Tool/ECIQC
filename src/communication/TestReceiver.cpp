#include "TestReceiver.hpp"

void TestReceiver::run() {
  Uint16 rspStatusCode = 0;
  negotiateAssociation();
  for (int i = 0; i < m_filenames.size(); i++) {
    m_results[i] =
        sendSTORERequest(0, m_filenames[i].c_str(), nullptr, rspStatusCode);
  }
  releaseAssociation();
}
