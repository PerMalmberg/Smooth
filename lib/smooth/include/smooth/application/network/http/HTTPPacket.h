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

#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>
#include <smooth/core/network/IPacketDisassembly.h>
#include <smooth/application/network/http/ResponseCodes.h>
#include "HTTPMethod.h"

namespace smooth::application::network::http
{
    class HTTPPacket
            : public smooth::core::network::IPacketDisassembly
    {
        public:

            HTTPPacket() = default;

            HTTPPacket(const HTTPPacket&) = default;

            HTTPPacket& operator=(const HTTPPacket&) = default;

            HTTPPacket(HTTPPacket&&) = default;

            HTTPPacket(ResponseCode code, const std::string& version,
                       const std::unordered_map<std::string, std::string>& new_headers,
                       const std::vector<uint8_t>& response_content);

            HTTPPacket(HTTPMethod method, const std::string& url,
                       const std::unordered_map<std::string, std::string>& new_headers,
                       const std::vector<uint8_t>& response_content);

            explicit HTTPPacket(std::vector<uint8_t>& response_content);


            // Must return the total amount of bytes to send
            int get_send_length() override
            {
                return static_cast<int>(content.size());
            }

            // Must return a pointer to the data to be sent.
            const uint8_t* get_data() override
            {
                return content.data();
            }

            const auto& get_buffer()
            {
                return content;
            }

            void set_continued()
            {
                continued = true;
            }

            bool is_continued() const
            {
                return continued;
            }

            void set_continuation()
            {
                continuation = true;
            }

            bool is_continuation() const
            {
                return continuation;
            }

            void
            set_request_data(const std::string& method, const std::string& url, const std::string& version)
            {
                request_method = method;
                request_url = url;
                request_version = version;
            }

            void set_response_data(ResponseCode code)
            {
                resp_code = code;
            }

            ResponseCode response_code() const
            {
                return resp_code;
            }

            const std::string& get_request_url() const
            {
                return request_url;
            }

            const std::string& get_request_method() const
            {
                return request_method;
            }

            const std::string& get_request_version() const
            {
                return request_version;
            }

            std::vector<uint8_t>& data()
            {
                return content;
            }

            void ensure_room(int total_space)
            {
                content.resize(static_cast<std::size_t>(total_space));
            }

            std::unordered_map<std::string, std::string>& headers()
            {
                return request_headers;
            }

            void clear()
            {
                content.clear();
            }

            auto find_header_ending() const
            {
                const auto end = std::search(content.cbegin(), content.cend(), ending.cbegin(), ending.cend());

                return end;
            }

            static constexpr std::array<uint8_t, 4> ending{'\r', '\n', '\r', '\n'};

        private:
            void append(const std::string& s);

            void add_header(const std::string& key, const std::string& value);

            std::unordered_map<std::string, std::string> request_headers{};
            std::string request_method{};
            std::string request_url{};
            std::string request_version{};
            std::vector<uint8_t> content{};
            ResponseCode resp_code{};
            bool continuation = false;
            bool continued = false;
    };
}