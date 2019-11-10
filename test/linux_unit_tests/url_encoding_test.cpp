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

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <array>
#include <unordered_map>

#include "smooth/application/network/http/URLEncoding.h"

using namespace smooth::application::network::http;

using Pair = std::pair<const char*, const char*>;

SCENARIO("Decoding")
{
    GIVEN("An encoded URL")
    {
        constexpr std::array<Pair, 5> valid_data =
        {
            std::make_pair("https://not-encoded.com", "https://not-encoded.com"),
            std::make_pair("https://www.example.com/åäö",
                                       "https%3A%2F%2Fwww.example.com%2F%C3%A5%C3%A4%C3%B6"),
            std::make_pair("https://www.example.com/?a=åäö",
                                       "https%3A%2F%2Fwww.example.com%2F?a=%C3%A5%C3%A4%C3%B6"),
            std::make_pair(
                                "/login?redirectToUrl=/ekonomi/din-ekonomi/ny-rattspraxis-grannar-har-ratt-att-overklaga-attefallare/",
                                "/login?redirectToUrl=%2Fekonomi%2Fdin-ekonomi%2Fny-rattspraxis-grannar-har-ratt-att-overklaga-attefallare%2F"),
            std::make_pair("/{}[]()!\"#¤%&/()=?",
                                       "%2F%7B%7D%5B%5D%28%29%21%22%23%C2%A4%25%26%2F%28%29%3D%3F")
                };

        constexpr std::array<Pair, 1> invalid_data =
        {
            std::make_pair("Missing last hex-digit",
                                       "https%3A%2F%2Fwww.example.com%2F%C3%A5%C3%A4%C3%B")
                };

        URLEncoding enc;
        WHEN("Decoding")
        {
            THEN("Decoded string matches")
            {
                for (auto const& p : valid_data)
                {
                    std::string encoded = p.second;
                    REQUIRE(enc.decode(encoded));
                    REQUIRE(encoded == p.first);
                }
            }
        }
        AND_WHEN("Decoding invalid URLs")
        {
            THEN("Decoding fails")
            {
                for (auto const& p : invalid_data)
                {
                    std::string encoded = p.second;
                    REQUIRE_FALSE(enc.decode(encoded));
                }
            }
        }
    }
}
