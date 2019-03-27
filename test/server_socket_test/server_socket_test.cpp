#include "server_socket_test.h"
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/Application.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/network/ServerSocket.h>
#include <iostream>
#include "wifi_creds.h"

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::network;
using namespace smooth::core::network::event;
using namespace smooth::core::logging;

namespace server_socket_test
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1)),
              client_connected("client_connected", 3, *this, *this),
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
        if (!server)
        {
            server = ServerSocket<StreamingProtocol>::create(client_connected,
                                                             tx_buffer,
                                                             rx_buffer,
                                                             tx_empty,
                                                             data_available,
                                                             connection_status,
                                                             seconds{1});

            server->start(std::make_shared<IPv4>("0.0.0.0", 8080));
        }
    }


    void App::event(const ClientConnectedEvent<StreamingProtocol>& ev)
    {
        auto client = ev.get_socket();
        tx_buffer.put(StreamPacket{'a'});


    }

    void App::event(const TransmitBufferEmptyEvent&)
    {

    }

    void App::event(const DataAvailableEvent<StreamingProtocol>& ev)
    {
        StreamingProtocol::packet_type packet;
        ev.get(packet);
        std::cout << static_cast<char>(packet.data()[0]);
    }

    void App::event(const ConnectionStatusEvent&)
    {

    }
}

