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

#include <smooth/application/network/http/websocket/WebsocketServer.h>

namespace http_server_test
{
    class WSEchoServer
            : public smooth::application::network::http::websocket::WebsocketServer
    {
        public:
            WSEchoServer(smooth::application::network::http::IServerResponse& response, smooth::core::Task& task)
                    : WebsocketServer(response, task)
            {
            }

            void data_received(bool first_part, bool last_part, bool is_text, const std::vector<uint8_t>& data) override;
    };
}