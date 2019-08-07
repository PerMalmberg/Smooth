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

#include <smooth/application/network/http/HTTPPacket.h>

#include <string>
#include <unordered_map>
#include <algorithm>
#include <smooth/application/network/http/http_utils.h>

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

    HTTPPacket::HTTPPacket(HTTPMethod method, const std::string& url,
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