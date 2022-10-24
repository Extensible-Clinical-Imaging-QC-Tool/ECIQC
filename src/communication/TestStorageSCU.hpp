#ifndef TESTSTORAGESCU_H
#define TESTSTORAGESCU_H

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/dstorscu.h>
#include <string>
#include <vector>

class TestStorageSCU : DcmStorageSCU, OFThread {
  std::vector<OFCondition> m_results;
  std::vector<std::string> m_filenames;
  std::string m_title;
  std::string m_peer_aetitle;
  std::string m_hostname;
  int m_port;

public:
  TestStorageSCU(std::string title, int port);
  void initialise();
  void add_file(const std::string &filename) {
    m_filenames.push_back(filename);
    m_results.resize(m_filenames.size());
  }
  void clear_files() {
    m_filenames.clear();
    m_results.clear();
  }
  const auto &get_filenames() { return m_filenames; };
  const auto &get_results() { return m_results; };

protected:
  void run() override;
};

#endif
