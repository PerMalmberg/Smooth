// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include <array>
#include <unordered_map>

#include <smooth/application/network/http/URLEncoding.h>

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