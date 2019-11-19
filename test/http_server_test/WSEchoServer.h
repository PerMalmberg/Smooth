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

#include "smooth/application/network/http/websocket/WebsocketServer.h"
#include "smooth/core/timer/Timer.h"
#include "smooth/core/ipc/TaskEventQueue.h"

namespace http_server_test
{
    class WSEchoServer
        : public smooth::application::network::http::websocket::WebsocketServer,
        smooth::core::ipc::IEventListener<smooth::core::timer::TimerExpiredEvent>
    {
        public:
            WSEchoServer(smooth::application::network::http::IServerResponse& response, smooth::core::Task& task);

            ~WSEchoServer() override = default;

            void data_received(bool first_part, bool last_part, bool is_text,
                               const std::vector<uint8_t>& data) override;

            void event(const smooth::core::timer::TimerExpiredEvent& event) override;

        private:
            std::shared_ptr<smooth::core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent>> timer_queue;
            smooth::core::timer::TimerOwner timer;
    };
}
