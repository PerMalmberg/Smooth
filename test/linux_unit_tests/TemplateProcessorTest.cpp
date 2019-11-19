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
#include <unordered_map>

#define EXPOSE_PRIVATE_PARTS_FOR_TEST

#include "smooth/application/network/http/regular/TemplateProcessor.h"
#include "smooth/application/network/http/regular/ITemplateDataRetriever.h"

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
        std::string text{ "Hello {{name}}, want {{food}}?" };

        THEN("Correctly replaces tokens")
        {
            auto dr = std::make_shared<DataRetriever>();
            std::set<std::string> ss{ "html" };

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
        std::string text{ "---{{abc}}---" };

        THEN("Replaces with empty string")
        {
            auto dr = std::make_shared<DataRetriever>();
            std::set<std::string> ss{ "html" };

            TemplateProcessor tp(std::move(ss), dr);
            tp.process_template(text);
            REQUIRE(text == "------");
        }
    }
}
