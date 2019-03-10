//
// Created by permal on 2018-10-21.
//

#include "secure_socket_test.h"
#include <iostream>
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/Application.h>
#include <smooth/core/logging/log.h>

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::timer;
using namespace smooth::core::network;
using namespace smooth::core::logging;

namespace secure_socket_test
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1)),
            tx_empty("tx_emtpy", 3, *this, *this),
            data_available("data_available", 3, *this, *this),
            connection_status("connection_status", 3, *this, *this)
    {
    }

    void App::tick()
    {
        if (!sock)
        {
            sock = SecureSocket<HTTPPacket>::create(tx_buffer, rx_buffer, tx_empty, data_available, connection_status);
            sock->start(std::make_shared<IPv4>("216.58.211.142" /*google.com*/, 443));
        }
    }

    void App::event(const smooth::core::network::TransmitBufferEmptyEvent&)
    {

    }

    void App::event(const smooth::core::network::DataAvailableEvent<HTTPPacket>& packet)
    {
        HTTPPacket p;
        packet.get(p);
        std::cout << p.get_status_line() << std::endl;
        sock->stop();
    }

    void App::event(const smooth::core::network::ConnectionStatusEvent& ev)
    {
        Log::info("Connection status: ", Format("{1}", Bool(ev.is_connected())));
        tx_buffer.put(HTTPPacket("GET / HTTP 1.0\r\nHost: www.google.com\r\n\r\n\r\n"));

    }



}