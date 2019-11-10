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

#include <regex>
#include "smooth/core/network/IPacketAssembly.h"
#include "smooth/application/network/http/HTTPPacket.h"
#include "smooth/application/network/http/IServerResponse.h"
#include "IUpgradeToWebsocket.h"

namespace smooth::application::network::http::regular
{
    class RegularHTTPProtocol
        : public smooth::core::network::IPacketAssembly<RegularHTTPProtocol, HTTPPacket>
    {
        public:
            using packet_type = HTTPPacket;

            RegularHTTPProtocol(int max_header_size,
                                int content_chunk_size,
                                IServerResponse& response,
                                IUpgradeToWebsocket& websocket_upgrade)
                    : max_header_size(max_header_size),
                      content_chunk_size(content_chunk_size),
                      response(response),
                      websocket_upgrade(websocket_upgrade)
            {
            }

            int get_wanted_amount(HTTPPacket& packet) override;

            void data_received(HTTPPacket& packet, int length) override;

            uint8_t* get_write_pos(HTTPPacket& packet) override;

            bool is_complete(HTTPPacket& packet) const override;

            bool is_error() override;

            void packet_consumed() override;

            void reset() override;

        private:
            int consume_headers(HTTPPacket& packet, std::vector<uint8_t>::const_iterator header_ending);

            enum class State
            {
                reading_headers,
                reading_content
            };

            const int max_header_size;
            const int content_chunk_size;
            IServerResponse& response;

            int total_bytes_received{ 0 };
            int total_content_bytes_received{ 0 };
            int content_bytes_received_in_current_part{ 0 };
            int incoming_content_length{ 0 };
            int actual_header_size{ 0 };

            const std::regex response_line{ R"!(HTTP\/(\d.\d)\ (\d+)\ (.+))!" }; // HTTP/1.1 200 OK
            const std::regex request_line{ R"!((.+)\ (.+)\ HTTP\/(\d\.\d))!" }; // "GET / HTTP/1.1"

            bool error = false;
            State state = State::reading_headers;
            std::string last_method{};
            std::string last_url{};

            std::string last_request_version{};
            IUpgradeToWebsocket& websocket_upgrade;
    };
}
