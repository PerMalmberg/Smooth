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

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>
#include "smooth/core/network/IPacketDisassembly.h"
#include "smooth/application/network/http/regular/ResponseCodes.h"
#include "regular/HTTPMethod.h"
#include "websocket/OpCode.h"

namespace smooth::application::network::http
{
    // This class acts as a carrier for two types of data:
    // 1: Regular HTTP response/request data
    // 2: Websocket data

    class HTTPPacket
        : public smooth::core::network::IPacketDisassembly
    {
        public:
            HTTPPacket() = default;

            HTTPPacket(const HTTPPacket&) = default;

            HTTPPacket& operator=(const HTTPPacket&) = default;

            HTTPPacket(HTTPPacket&&) = default;

            HTTPPacket(regular::ResponseCode code, const std::string& version,
                       const std::unordered_map<std::string, std::string>& new_headers,
                       const std::vector<uint8_t>& response_content);

            HTTPPacket(regular::HTTPMethod method, const std::string& url,
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

            void set_request_data(const std::string& method, const std::string& url, const std::string& version)
            {
                request_method = method;
                request_url = url;
                request_version = version;
            }

            void set_response_data(regular::ResponseCode code)
            {
                resp_code = code;
            }

            regular::ResponseCode response_code() const
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

            void expand_by(int additional_space)
            {
                content.resize(static_cast<std::size_t>(content.size()
                                                        + static_cast<decltype(content.size())>(additional_space)));
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

            void set_ws_control_code(websocket::OpCode code)
            {
                ws_opcode = code;
            }

            websocket::OpCode ws_control_code() const
            {
                return ws_opcode;
            }

            static constexpr std::array<uint8_t, 4> ending{ '\r', '\n', '\r', '\n' };
        private:
            void append(const std::string& s);

            void add_header(const std::string& key, const std::string& value);

            std::unordered_map<std::string, std::string> request_headers{};
            std::string request_method{};
            std::string request_url{};
            std::string request_version{};
            std::vector<uint8_t> content{};
            regular::ResponseCode resp_code{};
            bool continuation = false;
            bool continued = false;
            websocket::OpCode ws_opcode{ websocket::OpCode::Continuation };
    };
}
