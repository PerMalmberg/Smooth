/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2020 Per Malmberg (https://gitbub.com/PerMalmberg)

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

#include <memory>
#include <string>
#include <unordered_map>
#include "smooth/application/network/http/IServerResponse.h"
#include "smooth/application/network/http/IConnectionTimeoutModifier.h"

namespace smooth::application::network::http::regular
{
    class MIMEParser;

    /// Base class interface for all HTTP request handlers
    class IHTTPRequestHandler
    {
        public:
            virtual ~IHTTPRequestHandler() = default;

            /// Called for each part of the incoming data in the current request
            virtual void request(IServerResponse& response,
                                 IConnectionTimeoutModifier& timeout_modifier,
                                 const std::string& url,
                                 bool first_part, // True on the first call
                                 bool last_part, // True on the last call
                                 const std::unordered_map<std::string, std::string>& headers,
                                 const std::unordered_map<std::string, std::string>& request_parameters,
                                 const std::vector<uint8_t>& content,
                                 MIMEParser& mime) = 0;

            // Convenience methods for setting up the handler.
            virtual void start_of_request() {} // Called before the first call to request().

            virtual void end_of_request() {} // Called after the last call to request().
    };
}
