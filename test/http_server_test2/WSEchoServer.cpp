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

#include "smooth/application/network/http/IServerResponse.h"
#include "smooth/application/network/http/websocket/responses/WSResponse.h"
#include "smooth/application/network/http/websocket/WebsocketServer.h"
#include "WSEchoServer.h"

using namespace smooth::application::network::http::websocket::responses;
using namespace smooth::application::network::http::websocket;
using namespace smooth::core::ipc;

namespace http_server_test
{
    using namespace smooth::core::timer;
    using namespace std::chrono;

    WSEchoServer::WSEchoServer(smooth::application::network::http::IServerResponse& response, smooth::core::Task& task)
            : WebsocketServer(response, task),
              timer_queue(TaskEventQueue<TimerExpiredEvent>::create(1, task, *this)),
              timer(Timer::create(0, timer_queue, true, seconds {1}))
    {
        timer->start();
    }

    void http_server_test::WSEchoServer::data_received(bool first_part, bool last_part, bool is_text,
                                                       const std::vector<uint8_t>& data)
    {
        response.reply(std::make_unique<WSResponse>(data, is_text, first_part, last_part), false);
    }

    void WSEchoServer::event(const smooth::core::timer::TimerExpiredEvent&)
    {
        response.reply(std::make_unique<WSResponse>(OpCode::Ping), true);
    }
}
