#ifndef TESTRECEIVER_H
#define TESTRECEIVER_H

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/dstorscu.h>
#include <string>
#include <vector>

class TestReceiver : DcmStorageSCU, OFThread {
  std::vector<OFCondition> m_results;
  std::vector<std::string> m_filenames;

public:
  void add_file(const std::string& filename) {
    m_filenames.push_back(filename);
    m_results.resize(m_filenames.size());
  }
  void clear_files() {
    m_filenames.clear();
    m_results.clear();
  }
  auto get_filenames() -> std::vector<std::string> & { return m_filenames; };
  auto get_results() -> std::vector<OFCondition> & { return m_results; };

protected:
  void run() override;
};

#endif
