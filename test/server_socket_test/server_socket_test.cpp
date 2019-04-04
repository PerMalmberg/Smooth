#include "server_socket_test.h"
#include <deque>
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/Application.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/network/ServerSocket.h>
#include "wifi_creds.h"

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::network;
using namespace smooth::core::network::event;
using namespace smooth::core::logging;

namespace server_socket_test
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds(1000)),
              client_connected("client_connected", 3, *this, *this)

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
            server = ServerSocket<StreamingProtocol, StreamingClient>::create(client_connected, *this);
            server->start(std::make_shared<IPv4>("0.0.0.0", 8080));
        }
    }


    void App::event(const ClientConnectedEvent<StreamingClient>& ev)
    {
        auto client = ev.get_client();
        

        server->return_client_to_pool(client);
    }
}

