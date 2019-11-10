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

#include <memory>
#include "smooth/application/network/http/websocket/WebsocketServer.h"
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
