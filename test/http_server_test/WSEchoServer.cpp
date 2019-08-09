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

#include <smooth/application/network/http/IServerResponse.h>
#include <smooth/application/network/http/websocket/responses/WSResponse.h>
#include <smooth/application/network/http/websocket/WebsocketServer.h>
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
              timer_queue(TaskEventQueue<TimerExpiredEvent>::create("time_queue", 1, task, *this)),
              timer(Timer::create("Ping", 0, timer_queue, true, seconds{1}))
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