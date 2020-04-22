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
#include <string>
#include "RequestHandlerSignature.h"
#include "smooth/application/network/http/IServerResponse.h"
#include "smooth/application/network/http/IConnectionTimeoutModifier.h"

namespace smooth::application::network::http::regular
{
    class MIMEParser;

    class IRequestHandler
    {
        public:
            virtual ~IRequestHandler() = default;

            virtual void handle(HTTPMethod method,
                                IServerResponse& response,
                                IConnectionTimeoutModifier& timeout_modifier,
                                const std::string& requested_url,
                                const std::unordered_map<std::string, std::string>& request_headers,
                                const std::unordered_map<std::string, std::string>& request_parameters,
                                const std::vector<uint8_t>& data,
                                bool fist_part,
                                bool last_part) = 0;
    };
}
