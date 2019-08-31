// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.smooth::application::network::http
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

#include <memory>
#include <smooth/application/network/http/websocket/WebsocketServer.h>
#include "smooth/application/network/http/IResponseOperation.h"

namespace smooth::application::network::http
{
    class IServerResponse
    {
        public:
            virtual ~IServerResponse() = default;

            virtual void reply(std::unique_ptr<IResponseOperation> response, bool place_first) = 0;

            virtual void reply_error(std::unique_ptr<IResponseOperation> response) = 0;

            template<typename WSServerType>
            void upgrade_to_websocket()
            {
                upgrade_to_websocket_internal();
                ws_server = std::make_unique<WSServerType>(*this, get_task());
            }

        protected:
            virtual smooth::core::Task& get_task() = 0;

            virtual void upgrade_to_websocket_internal() = 0;

            std::unique_ptr<websocket::WebsocketServer> ws_server{};
    };
}
