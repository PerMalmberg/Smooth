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
#include <memory>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/IPacketDisassembly.h>
#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/core/util/string_util.h>
#include "HTTPPacket.h"
#include "regular/HTTPHeaderDef.h"
#include "IServerResponse.h"
#include "regular/RegularHTTPProtocol.h"
#include "websocket/WebsocketProtocol.h"

using namespace smooth::core;
using namespace smooth::core::logging;

namespace smooth::application::network::http
{
    using namespace smooth::application::network::http::regular;

    class HTTPProtocol
            : public smooth::core::network::IPacketAssembly<HTTPProtocol, HTTPPacket>,
              public IUpgradeToWebsocket
    {
        public:
            using packet_type = HTTPPacket;

            HTTPProtocol(int max_header_size, int content_chunk_size, IServerResponse& response)
                    : max_header_size(max_header_size),
                      content_chunk_size(content_chunk_size),
                      response(response),
                      regular(std::make_unique<RegularHTTPProtocol>(max_header_size,
                                                                    content_chunk_size,
                                                                    response,
                                                                    *this))
            {
            }

            int get_wanted_amount(HTTPPacket& packet) override;

            void data_received(HTTPPacket& packet, int length) override;

            uint8_t* get_write_pos(HTTPPacket& packet) override;

            bool is_complete(HTTPPacket& packet) const override;

            bool is_error() override;

            void packet_consumed() override;

            void reset() override;

            void upgrade_to_websocket() override;

        private:

            const int max_header_size;
            const int content_chunk_size;
            IServerResponse& response;
            std::unique_ptr<RegularHTTPProtocol> regular{};
            std::unique_ptr<websocket::WebsocketProtocol> websocket{};
    };
}


