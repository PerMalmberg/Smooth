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

#include "smooth/application/network/http/HTTPPacket.h"

#include <string>
#include <unordered_map>
#include <algorithm>
#include "smooth/application/network/http/http_utils.h"

namespace smooth::application::network::http
{
    using namespace smooth::application::network::http::regular;

    HTTPPacket::HTTPPacket(ResponseCode code, const std::string& version,
                           const std::unordered_map<std::string, std::string>& new_headers,
                           const std::vector<uint8_t>& response_content)
    {
        append("HTTP/");
        append(version);
        append(" ");
        append(std::to_string(static_cast<int>(code)));
        append(" ");
        append(response_code_to_text.at(code));
        append("\r\n");

        for (const auto& header : new_headers)
        {
            const auto& key = header.first;
            const auto& value = header.second;

            add_header(key, value);
        }

        // Add required ending CRLF
        append("\r\n");

        if (!response_content.empty())
        {
            std::copy(response_content.begin(), response_content.end(), std::back_inserter(content));
        }
    }

    HTTPPacket::HTTPPacket(HTTPMethod method,
                           const std::string& url,
                           const std::unordered_map<std::string, std::string>& new_headers,
                           const std::vector<uint8_t>& response_content)
    {
        append(utils::http_method_to_string(method));
        append(" ");
        append(url);
        append(" HTTP/1.1");
        append("\r\n");

        for (const auto& header : new_headers)
        {
            const auto& key = header.first;
            const auto& value = header.second;

            add_header(key, value);
        }

        // Add required ending CRLF
        append("\r\n");

        if (!response_content.empty())
        {
            std::copy(response_content.begin(), response_content.end(), std::back_inserter(content));
        }
    }

    void HTTPPacket::append(const std::string& s)
    {
        std::copy(s.begin(), s.end(), std::back_inserter(content));
    }

    void HTTPPacket::add_header(const std::string& key, const std::string& value)
    {
        append(key);
        append(": ");
        append(value);
        append("\r\n");
    }

    HTTPPacket::HTTPPacket(std::vector<uint8_t>& response_content)
    {
        content = std::move(response_content);
    }
}
