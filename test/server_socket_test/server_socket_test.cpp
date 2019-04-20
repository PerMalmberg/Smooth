#include "server_socket_test.h"
#include <deque>
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/Application.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/network/ServerSocket.h>
#include <smooth/core/network/SecureServerSocket.h>
#include "wifi_creds.h"

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::network;
using namespace smooth::core::network::event;
using namespace smooth::core::logging;

namespace server_socket_test
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds(1000))

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

        // The server creates StreamingClients which are self-sufficient and never seen by the main
        // application (unless the implementor adds such bindings).
        server = ServerSocket<StreamingClient, StreamingProtocol>::create(*this, 5);
        server->start(std::make_shared<IPv4>("0.0.0.0", 8080));

        // Point your browser to http://localhost:8080 and watch the output.
        // Or, if you're on linux, do "echo ` date` | nc localhost 8080 -w1"
    }

}

