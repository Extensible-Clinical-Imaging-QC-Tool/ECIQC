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
#include "dcmtk/config/osconfig.h"
#include <dcmtk/dcmdata/dcdatset.h> 
#include <dcmtk/dcmdata/dcdict.h>
#include "catch.hpp"

#include <limits>
#include <type_traits>

#include "../src/Exception.hpp"
#include "../src/MyLibrary.hpp"

#include "../src/draft_receiver.hpp"
#include "../src/draft_receiver.cpp"
#include <iostream>

  /* make sure OS specific configuration is included first */

#ifdef WITH_THREADS

#include <dcmtk/ofstd/oftest.h>
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <dcmtk/dcmnet/scppool.h>
#include <dcmtk/dcmnet/scu.h>

using namespace cpp_template;


// This tests the output of the `get_nth_prime` function
TEST_CASE("correct primes are returned", "[primes]") {

  //GlobalDcmDataDictionary dcmDataDict(OFTrue /*load builtin*/, OFFalse);

  std::cout << "I work now"<<'\n'; 
  

struct TestSCU : DcmSCU, OFThread
{
    OFCondition result;
protected:
    void run()
    {
        negotiateAssociation();
        result = sendECHORequest(0);
        releaseAssociation();
    }
};

StorageServer pool;
DcmSCPConfig& config = pool.getConfig();

//config.setAETitle("PoolTestSCP");
//config.setPort(11112);
//config.setConnectionBlockingMode(DUL_NOBLOCK);

std::cout<<"port is "<<config.getPort()<<'\n';
std::cout<<"name is "<<config.getAETitle()<<'\n';
std::cout<<"name is "<<config.getConnectionTimeout()<<'\n';
//config.setConnectionBlockingMode(DUL_NOBLOCK);

//config.setConnectionTimeout(1);

//pool.setMaxThreads(2);

/*OFList<OFString> xfers;
xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
    
config.addPresentationContext(UID_VerificationSOPClass, xfers); */
pool.start();
OFStandard::sleep(10);
/*OFVector<TestSCU*> scus(2);
for (OFVector<TestSCU*>::iterator it1 = scus.begin(); it1 != scus.end(); ++it1)
    {
        *it1 = new TestSCU;
        (*it1)->setAETitle("PoolTestSCU");
        (*it1)->setPeerAETitle("TestSCP");
        (*it1)->setPeerHostName("localhost");
        (*it1)->setPeerPort(11112);
        (*it1)->addPresentationContext(UID_VerificationSOPClass, xfers);
        (*it1)->initNetwork();
        std::cout<<"something happened"<<'\n';
    }


  OFStandard::sleep(5); 
  


  for (OFVector<TestSCU*>::const_iterator it2 = scus.begin(); it2 != scus.end(); ++it2)
      (*it2)->start();


  for (OFVector<TestSCU*>::iterator it3 = scus.begin(); it3 != scus.end(); ++it3)
    {
      (*it3)->join();
      if(!(*it3)->result.good()){
        std::cout<<"Something failed"<<'\n';
      }
      delete *it3;
    };
    // Request shutdown. */
  pool.request_stop();
  pool.join();

}
  

#endif

