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

#include <cstdint>
#include <regex>
#include <memory>
#include "smooth/core/logging/log.h"
#include "smooth/core/network/IPacketDisassembly.h"
#include "smooth/core/network/IPacketAssembly.h"
#include "smooth/core/util/string_util.h"
#include "HTTPPacket.h"
#include "regular/HTTPHeaderDef.h"
#include "IServerResponse.h"
#include "regular/RegularHTTPProtocol.h"
#include "websocket/WebsocketProtocol.h"

using namespace smooth::core::logging;
using namespace smooth::core;

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
