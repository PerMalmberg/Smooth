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

#include "SendBlob.h"
#include "smooth/application/network/http/regular/IHTTPRequestHandler.h"

namespace http_server_test
{
    class BlobResponder : public smooth::application::network::http::regular::IHTTPRequestHandler
    {
        public:
            void request(smooth::application::network::http::IServerResponse& response,
                         smooth::application::network::http::IConnectionTimeoutModifier& timeout_modifier,
                         const std::string& url,
                         bool first_part,
                         bool last_part,
                         const std::unordered_map<std::string, std::string>& headers,
                         const std::unordered_map<std::string, std::string>& request_parameters,
                         const std::vector<uint8_t>& content) override;
    };
}
