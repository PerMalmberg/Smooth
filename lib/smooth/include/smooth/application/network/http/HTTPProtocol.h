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

#include <cstdint>
#include <regex>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/IPacketDisassembly.h>
#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/core/util/string_util.h>
#include "regular/HTTPPacket.h"
#include "regular/HTTPHeaderDef.h"
#include "regular/IServerResponse.h"

using namespace smooth::core;
using namespace smooth::core::logging;

namespace smooth::application::network::http
{

    using namespace smooth::application::network::http::regular;

    class HTTPProtocol
            : public smooth::core::network::IPacketAssembly<HTTPProtocol, HTTPPacket>
    {
            /*qqq static_assert(MaxHeaderSize > 0,
                          "MaxHeaderSize must be larger than 0, and a reasonable value.");
            static_assert(ContentChunkSize > 0,
                          "ContentChunkSize must be larger than 0, and a reasonable value.");
            static_assert(ContentChunkSize >= MaxHeaderSize,
                          "ContentChunkSize must be  larger or equal to MaxHeaderSize"); */

        public:
            using packet_type = HTTPPacket;

            HTTPProtocol(int max_header_size, int content_chunk_size, IServerResponse& response)
                    : max_header_size(max_header_size),
                      content_chunk_size(content_chunk_size),
                      response(response)
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

            int total_bytes_received{0};
            int total_content_bytes_received{0};
            int content_bytes_received_in_current_part{0};
            int incoming_content_length{0};
            int actual_header_size{0};

            const std::regex response_line{R"!(HTTP\/(\d.\d)\ (\d+)\ (.+))!"}; // HTTP/1.1 200 OK
            const std::regex request_line{R"!((.+)\ (.+)\ HTTP\/(\d\.\d))!"}; // "GET / HTTP/1.1"

            bool error = false;
            State state = State::reading_headers;
            std::string last_method{};
            std::string last_url{};

            std::string last_request_version{};
    };
}


