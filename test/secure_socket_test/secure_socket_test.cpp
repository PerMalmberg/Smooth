//
// Created by permal on 2018-10-21.
//

#include "secure_socket_test.h"
#include <iostream>
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/Application.h>
#include <smooth/core/logging/log.h>
#include "HTTPPacket.h"
#include "wifi_creds.h"

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

    void App::init()
    {
        Application::init();
#ifdef ESP_PLATFORM
        Log::info("App::Init", Format("Starting wifi..."));
        network::Wifi& wifi = get_wifi();
        wifi.set_host_name("Smooth-ESP");
        wifi.set_auto_connect(true);
        wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
        wifi.connect_to_ap();
#endif
    }

    void App::tick()
    {
        if (!sock)
        {
            sock = SecureSocket<HTTPProtocol<>>::create(tx_buffer, rx_buffer, tx_empty, data_available, connection_status);
            sock->start(std::make_shared<IPv4>("www.google.com", 443));
        }
    }

    void App::event(const smooth::core::network::TransmitBufferEmptyEvent&)
    {

    }

    void App::event(const smooth::core::network::DataAvailableEvent<HTTPProtocol<>>& packet)
    {
        HTTPProtocol<>::packet_type p;
        packet.get(p);

        if(!p.is_continued())
        {
            sock->stop();
        }
        //Log::debug("Status:", p.get_status_line());
    }

    void App::event(const smooth::core::network::ConnectionStatusEvent& ev)
    {
        Log::info("Connection status: ", Format("{1}", Bool(ev.is_connected())));
        tx_buffer.put(HTTPPacket("GET / HTTP/1.0\r\nHost: www.google.com\r\n\r\n\r\n"));
    }
}