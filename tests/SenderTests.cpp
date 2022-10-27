#include "catch.hpp"
#include "communication/ThreadSafeQueue.hpp"
#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/oflog/fileap.h"

#include "../src/misc/Exception.hpp"
#include "../src/misc/MyLibrary.hpp"

#include "../src/communication/Receiver.hpp"
#include "../src/communication/Sender.hpp"
/*

using namespace cpp_template;

#define OFFIS_CONSOLE_APPLICATION "my_Logger"

static OFLogger my_Logger = OFLog::getLogger("dcmtk.apps."
OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";
  */

TEST_CASE("Test C-ECHO Request with SCU", "[ST]") {
  std::string ae_title = "TEST-SCU";
  std::string peer_hostname = "www.dicomserver.co.uk";
  Uint16 peer_port = 11112;
  std::string peer_aetitle = "MOVESCP";
  Sender scu(ae_title, peer_hostname, peer_port, peer_aetitle);

  auto result = scu.send_echo();
  CHECK(result.good());
}

TEST_CASE("Test Unsuccessful C-STORE Association with SCU", "[STS]") {
  OFshared_ptr<ThreadSafeQueue<DcmDataset>> pt(new ThreadSafeQueue<DcmDataset>);
  Receiver pool(104, "MOVESCP");
  pool.setpointer(pt);
  pool.start();

  /* Setup DICOM connection parameters */
  std::string ae_title = "TEST-SCU"; /*"StoreTestSCU";*/
  std::string peer_hostname = "www.dicomserver.co.uk";
  Uint16 peer_port = 104;
  std::string peer_aetitle = "MOVESCP"; /*"TestSCP";*/
  Sender scu(ae_title, peer_hostname, peer_port, peer_aetitle);

  DcmFileFormat dfile;
  auto result = dfile.loadFile("../DICOM_Images/test2.dcm");
  CHECK(result.good());
  auto *data = dfile.getDataset();
  auto status = scu.send(*data);
  CHECK(status.good());

  // now we will send a nonexistant file
  /* Negotiate Association */ 
  status = scu.initNetwork(); 
  result = scu.negotiateAssociation(); 
  CHECK(result.good());

  /*Assemble and send C-STORE request. Check if C-STORE was successful.*/
  Uint16 rspStatusCode = 0;
  result = scu.sendSTORERequest(0, "../DICOM_Images/test1",0, rspStatusCode = 0);
  CHECK(result.bad());

  /*Release association. */
  result = scu.releaseAssociation();
  CHECK(result.good());

  /*Request shutdown and stop listening. */
  pool.request_stop();
  pool.join();
}

TEST_CASE("Test Successful C-STORE Association with SCU", "[STS2]") {
  OFshared_ptr<ThreadSafeQueue<DcmDataset>> pt(new ThreadSafeQueue<DcmDataset>);
  Receiver pool(104, "MOVESCP");
  pool.setpointer(pt);
  // Start listening
  pool.start();

  /* Setup DICOM connection parameters */
  std::string ae_title = "TEST-SCU"; /*"StoreTestSCU";*/
  std::string peer_hostname = "www.dicomserver.co.uk";
  Uint16 peer_port = 104;
  std::string peer_aetitle = "MOVESCP"; /*"TestSCP";*/
  Sender scu(ae_title, peer_hostname, peer_port, peer_aetitle);

  auto result = scu.send_file("../DICOM_Images/1-01.dcm");
  CHECK(result.good());

  /*Extracting data from dicom file.*/
  DcmFileFormat dfile;
  result = dfile.loadFile("../DICOM_Images/1-01.dcm");
  CHECK(result.good());

  DcmDataset *data = dfile.getDataset();
  CHECK(data != NULL);

  result = scu.send(*data);
  CHECK(result.good());
}
