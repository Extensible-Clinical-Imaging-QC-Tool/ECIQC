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
#include "catch.hpp"

#include <limits>
#include <type_traits>

#include "Exception.hpp"
#include "MyLibrary.hpp"

#include "../parser.cpp"

using namespace cpp_template;

// This tests the output of the `get_nth_prime` function
TEST_CASE("correct primes are returned", "[primes]") {
  CHECK(get_nth_prime(0) == 2);
  CHECK(get_nth_prime(1) == 3);
  CHECK(get_nth_prime(2) == 5);
  CHECK(get_nth_prime(854) == 6619);
}

// This tests the correct out_of_range exceptions are generated
TEST_CASE("correct out of range exceptions", "[primes]") {
  CHECK_THROWS_AS(get_nth_prime(-1), Exception);
  CHECK_THROWS_AS(get_nth_prime(std::numeric_limits<int>::max()), Exception);
}
// This tests the size of the tags array generated 
TEST_CASE("test the function extractor of the Instances Class") {
  //Instances object;
  //CHECK(object.function_extractor().tags.size() == 20);
}
