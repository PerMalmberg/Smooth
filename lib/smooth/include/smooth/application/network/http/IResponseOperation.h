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

#include <unordered_map>
#include "smooth/core/network/BufferContainer.h"
#include "smooth/application/network/http/regular/ResponseCodes.h"

namespace smooth::application::network::http
{
    enum class ResponseStatus
    {
        Error,
        HasMoreData,
        LastData,
        NoData
    };

    // A request operation is responsible for providing outgoing data chunked into smaller pieces
    // as per the passed arguments.
    class IResponseOperation
    {
        public:
            virtual ~IResponseOperation() = default;

            virtual regular::ResponseCode get_response_code()
            {
                return regular::ResponseCode::Not_Found;
            }

            const std::unordered_map<std::string, std::string>& get_headers() const
            {
                return headers;
            }

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            virtual ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) = 0;

            /// Sets a header, replacing any existing value
            virtual void set_header(const std::string& /*key*/, const std::string& /*value*/)
            {}

            /// Adds a header value, appending to any existing value.
            virtual void add_header(const std::string& /*key*/, const std::string& /*value*/)
            {}

            virtual void dump() const
            {}
        protected:
            std::unordered_map<std::string, std::string> headers{};
    };
}
