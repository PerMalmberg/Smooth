/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <catch2/catch.hpp>
#include <cstdint>
#include "smooth/application/hash/sha.h"
#include "smooth/application/hash/base64.h"

using namespace smooth::application::hash;

SCENARIO("Testing SHA1")
{
    GIVEN("Data")
    {
        const auto data = reinterpret_cast<const uint8_t*>("QWERTY12345");

        THEN("Correctly hashed")
        {
            auto sha1_encoded = sha1(data, 11);
            std::array<uint8_t, 20> expected{ 0x72, 0x66, 0xa3, 0xdd, 0x13, 0x7e, 0xb1, 0xe9, 0x19, 0x87, 0xec, 0x99,
                                              0x3a, 0xc3, 0x93, 0x87, 0xa7, 0x94, 0x52, 0x2f };
            REQUIRE(std::equal(sha1_encoded.begin(), sha1_encoded.end(), expected.begin(), expected.end()));
        }
    }
}

SCENARIO("Testing SHA256")
{
    GIVEN("Data")
    {
        const auto data =
            reinterpret_cast<const uint8_t*>("igreakljgdaklgraejoi4qe90ut34  jgrewgkl4qeogi53qojiygj34yg");

        THEN("Correctly hashed")
        {
            auto sha256_encoded = sha256(data, 58);
            std::array<uint8_t, 32> expected{ 0x46, 0xda, 0x77, 0x1b, 0xf8, 0xe5, 0x88, 0x6c, 0x17, 0xa8, 0x1a, 0xba,
                                              0x9a, 0x43, 0xe5, 0x97, 0xb8, 0x56, 0x98, 0x88, 0xf5, 0x68, 0x7f, 0x9c,
                                              0x2f, 0xc0, 0xa4, 0x18, 0x0d, 0x56, 0x7c, 0x69 };
            REQUIRE(std::equal(sha256_encoded.begin(), sha256_encoded.end(), expected.begin(), expected.end()));
        }
    }
}

SCENARIO("Testing base64")
{
    GIVEN("Data")
    {
        THEN("Correctly encoded")
        {
            REQUIRE("QUJDREVG" == base64::encode(reinterpret_cast<const uint8_t*>("ABCDEF"), 6));
            REQUIRE("YXNkZmprbG9maXNhZmxqd2xmandmbGp3aWdqcmV3Z25yZXdpMjQ0ZzM=" ==
                    base64::encode(reinterpret_cast<const uint8_t*>("asdfjklofisafljwlfjwfljwigjrewgnrewi244g3"), 41));
        }
    }
}
