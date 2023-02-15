#ifndef TESTSTORAGESCU_H
#define TESTSTORAGESCU_H

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/dstorscu.h>
#include <string>
#include <utility>
#include <vector>

class TestStorageSCU : private DcmStorageSCU, public OFThread {
  std::vector<OFCondition> m_results;
  OFCondition m_negotiate_association;
  OFCondition m_echo_result;
  OFCondition m_release_association;
  std::vector<std::string> m_filenames;
  std::string m_title;
  std::string m_peer_aetitle;
  std::string m_peer_hostname;
  int m_peer_port;

public:
  TestStorageSCU(std::string title, std::string peer_title, int port);
  TestStorageSCU();
  TestStorageSCU(const TestStorageSCU &other)
      : m_results(other.m_results),
        m_negotiate_association(other.m_negotiate_association),
        m_echo_result(other.m_echo_result),
        m_release_association(other.m_release_association),
        m_filenames(other.m_filenames), m_title(other.m_title),
        m_peer_aetitle(other.m_peer_aetitle),
        m_peer_hostname(other.m_peer_hostname),
        m_peer_port(other.m_peer_port){};

  TestStorageSCU &operator=(const TestStorageSCU &other) {
    if (this != &other) {
      std::copy(std::begin(other.m_results), std::end(other.m_results),
                std::begin(m_results));
      std::copy(std::begin(other.m_filenames), std::end(other.m_filenames),
                std::begin(m_filenames));

      m_negotiate_association = other.m_negotiate_association;
      m_echo_result = other.m_echo_result;
      m_release_association = other.m_release_association;
      m_filenames = other.m_filenames;
      m_title = other.m_title;
      m_peer_aetitle = other.m_peer_aetitle;
    }
    return *this;
  }
  void initialise();
  void add_file(const std::string &filename) {
    m_filenames.push_back(filename);
    m_results.resize(m_filenames.size());
  }
  void clear_files() {
    m_filenames.clear();
    m_results.clear();
  }
  void set_title(const std::string &title) { m_title = title; }
  void set_peer(const std::string &title, const int port,
                const std::string &hostname = "localhost") {
    m_peer_aetitle = title;
    m_peer_hostname = hostname;
    m_peer_port = port;
  }
  void set_peer_title(const std::string &title) { m_peer_aetitle = title; }
  const auto &get_filenames() { return m_filenames; };
  const auto &get_results() { return m_results; };
  const auto &get_neg_association_result() { return m_negotiate_association; };
  const auto &get_release_association_result() {
    return m_release_association;
  };
  const auto &get_echo_result() { return m_echo_result; };
  void run() override;
};

#endif
