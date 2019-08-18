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

#include "catch.hpp"
#include <unordered_map>

#define EXPOSE_PRIVATE_PARTS_FOR_TEST

#include <smooth/application/network/http/regular/TemplateProcessor.h>
#include <smooth/application/network/http/regular/ITemplateDataRetriever.h>

using namespace smooth::application::network::http::regular;

class DataRetriever
        : public ITemplateDataRetriever
{
    public:
        DataRetriever()
        {
            data.emplace("{{name}}", "Bob");
            data.emplace("{{food}}", "an ice cream");
        }

        std::string get(const std::string& key) const override
        {
            std::string res{};
            try
            {
                res = data.at(key);
            }
            catch (...)
            {

            }

            return res;
        }

    private:
        std::unordered_map<std::string, std::string> data{};
};


SCENARIO("Parsing a text")
{
    GIVEN("A text")
    {
        std::string text{"Hello {{name}}, want {{food}}?"};

        THEN("Correctly replaces tokens")
        {
            DataRetriever dr;
            std::set<std::string> ss{"html"};

            TemplateProcessor tp(std::move(ss), dr);
            tp.process_template(text);
            REQUIRE(text == "Hello Bob, want an ice cream?");
        }
    }
}

SCENARIO("Parsing a text with unknown tokens")
{
    GIVEN("A text")
    {
        std::string text{"---{{abc}}---"};

        THEN("Replaces with empty string")
        {
            DataRetriever dr;
            std::set<std::string> ss{"html"};

            TemplateProcessor tp(std::move(ss), dr);
            tp.process_template(text);
            REQUIRE(text == "------");
        }
    }
}