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

#include "smooth/application/network/http/IResponseOperation.h"

namespace smooth::application::network::http::regular::responses
{
    class HeaderOnlyResponse
        : public IResponseOperation
    {
        public:
            explicit HeaderOnlyResponse(ResponseCode code);

            HeaderOnlyResponse& operator=(HeaderOnlyResponse&&) = default;

            HeaderOnlyResponse(HeaderOnlyResponse&&) = default;

            HeaderOnlyResponse& operator=(const HeaderOnlyResponse&) = delete;

            HeaderOnlyResponse(const HeaderOnlyResponse&) = delete;

            ~HeaderOnlyResponse() override = default;

            ResponseCode get_response_code() override;

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;

            void set_header(const std::string& key, const std::string& value) override;

            void add_header(const std::string& key, const std::string& value) override;

            void dump() const override;

        protected:
            ResponseCode code;
    };
}
