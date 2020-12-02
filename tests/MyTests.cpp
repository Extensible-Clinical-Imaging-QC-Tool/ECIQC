/*
Copyright (c) 2018, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of the Oxford RSE C++ Template project.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include <limits>
#include <type_traits>
#include <iostream>
#include "catch.hpp"


#include <dcmtk/config/osconfig.h>

#ifdef WITH_THREADS

#include <dcmtk/dcmpstat/dcmpstat.h>
#include <dcmtk/dcmnet/scu.h>
#include <dcmtk/dcmnet/dstorscu.h>



#include "../src/Exception.hpp"
#include "../src/MyLibrary.hpp"

#include "../src/draft_receiver.hpp"


#include "../src/MetadataEditor.cpp"
#include "../src/MetadataEditor.hpp"

using namespace cpp_template;

TEST_CASE("Test for reading in a known DICOM image file") {
  DcmFileFormat image;
  OFString patientName;
  OFCondition status = image.loadFile("../DICOM_Images/1-1copy.dcm");

  CHECK(status.good() == 1);
  CHECK(image.getDataset()->findAndGetOFString(DCM_PatientName, patientName).good() == true);
  CHECK(patientName == "COVID-19-AR-16406488");
}

TEST_CASE("Testing the metadata editing class") {
  int hh = 3;
  MetadataEditor obj1;
  MetadataEditor obj2{hh};

  // Test instantiation with 0 arguments
  CHECK(obj1.x == 23);
  // Instantiation with 1 argument
  CHECK(obj2.x == hh);
}

// Testing Receiver Class

TEST_CASE("Test C-ECHO Assosciation"){

  struct TestSCU : DcmSCU, OFThread {
      OFCondition result;
    protected:
      void run(){
        negotiateAssociation();
        result = sendECHORequest(0);
        releaseAssociation();
        }
    };
  
  Receiver pool;

  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);

  pool.start();

  OFVector<TestSCU*> scus(2);
  for (OFVector<TestSCU*>::iterator it1 = scus.begin(); it1 != scus.end(); ++it1)
      {
          *it1 = new TestSCU;
          (*it1)->setAETitle("PoolTestSCU");
          (*it1)->setPeerAETitle("TestSCP");
          (*it1)->setPeerHostName("localhost");
          (*it1)->setPeerPort(11112);
          (*it1)->addPresentationContext(UID_VerificationSOPClass, xfers);
          (*it1)->initNetwork();
      }


    OFStandard::sleep(5); 
  


    for (OFVector<TestSCU*>::const_iterator it2 = scus.begin(); it2 != scus.end(); ++it2)
        (*it2)->start();


    for (OFVector<TestSCU*>::iterator it3 = scus.begin(); it3 != scus.end(); ++it3)
      {
        (*it3)->join();
        CHECK((*it3)->result.good());
        delete *it3;
      };
    // Request shutdown.
    pool.request_stop();
    pool.join();

}

TEST_CASE("Test C-STORE Assosciation"){

  struct TestSCU : DcmStorageSCU, OFThread {
      OFCondition result;
    protected:
      void run(){
        negotiateAssociation();
        DcmDataset * rspCommandSet = NULL;
        DcmDataset * rspStatusDetail = NULL;
        Uint16 rspStatusCode = 0;

        result = sendSTORERequest(0, "../DICOM_Images/1-01.dcm", 0, rspStatusCode);
        releaseAssociation();
        }
    };

  Receiver pool;

  OFList<OFString> xfers;
  xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
  xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
  //xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
  xfers.push_back(UID_JPEGProcess14SV1TransferSyntax);
  xfers.push_back(UID_JPEGProcess1TransferSyntax);

  pool.start();

  OFVector<TestSCU*> scus(2);
  for (OFVector<TestSCU*>::iterator it1 = scus.begin(); it1 != scus.end(); ++it1)
      {
          *it1 = new TestSCU;
          (*it1)->setAETitle("EchoTestSCU");
          (*it1)->setPeerAETitle("TestSCP");
          (*it1)->setPeerHostName("localhost");
          (*it1)->setPeerPort(11112);
          (*it1)->addPresentationContext(UID_DigitalXRayImageStorageForPresentation, xfers);
          for (size_t n = 0; n < numberOfDcmLongSCUStorageSOPClassUIDs; n++)
        {
            (*it1)->addPresentationContext(dcmLongSCUStorageSOPClassUIDs[n], xfers);
        }
          (*it1)->addPresentationContext(UID_VerificationSOPClass, xfers);
          (*it1)->initNetwork();
      }


    OFStandard::sleep(5); 
  


    for (OFVector<TestSCU*>::const_iterator it2 = scus.begin(); it2 != scus.end(); ++it2)
        (*it2)->start();


    for (OFVector<TestSCU*>::iterator it3 = scus.begin(); it3 != scus.end(); ++it3)
      {
        (*it3)->join();
        CHECK((*it3)->result.good());
        delete *it3;
      };
    // Request shutdown.
    pool.request_stop();
    pool.join();

}
  
// TODO: c-store with the x-ray image doesn't work :(
#endif

