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

#include "smooth/core/Task.h"
#include "smooth/application/network/http/websocket/responses/WSResponse.h"

namespace smooth::application::network::http
{
    class IServerResponse;
}

namespace smooth::application::network::http::websocket
{
    class WebsocketServer
    {
        public:
            WebsocketServer(smooth::application::network::http::IServerResponse& response, smooth::core::Task& task)
                    : response(response), task(task)
            {
            }

            virtual ~WebsocketServer() = default;

            virtual void data_received(bool first_part, bool last_part, bool is_text,
                                       const std::vector<uint8_t>& data) = 0;

            void close_connection();

        protected:
            IServerResponse& response;
            smooth::core::Task& task;
    };
}
