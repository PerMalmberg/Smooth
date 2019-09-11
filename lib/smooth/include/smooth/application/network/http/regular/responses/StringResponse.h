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

#pragma once

#include "HeaderOnlyResponse.h"
#include <vector>

namespace smooth::application::network::http::regular::responses
{
    class StringResponse
        : public HeaderOnlyResponse
    {
        public:
            explicit StringResponse(ResponseCode code, std::string body = "", bool add_surrounding_html = true);

            StringResponse& operator=(StringResponse&&) = default;

            StringResponse(StringResponse&&) = default;

            StringResponse& operator=(const StringResponse&) = delete;

            StringResponse(const StringResponse&) = delete;

            ~StringResponse() override = default;

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;

            void dump() const override;

        private:
            std::vector<uint8_t> data{};

            void add_string(std::string str);
    };
}
