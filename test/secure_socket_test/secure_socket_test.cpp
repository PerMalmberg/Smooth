#include "secure_socket_test.h"
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/Application.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/BufferContainer.h>
#include <cassert>
#include "HTTPPacket.h"
#include "wifi_creds.h"

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::timer;
using namespace smooth::core::network;
using namespace smooth::core::network::event;
using namespace smooth::core::logging;

namespace secure_socket_test
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1))
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
            buff = std::make_shared<BufferContainer<HTTPProtocol<>>>(*this, *this, *this, *this);
            sock = SecureSocket<HTTPProtocol<>>::create(buff);
            sock->start(std::make_shared<IPv4>("ftp.sunet.se", 443));
        }
    }

    void App::event(const TransmitBufferEmptyEvent&)
    {

    }

    void App::event(const DataAvailableEvent<HTTPProtocol<>>& packet)
    {
        HTTPProtocol<>::packet_type p;
        packet.get(p);

        if (!p.is_continuation())
        {
            // First packet
            assert(p.status() == 200);
        }
        else
        {
            // Seconds and onwards
            received_content.insert(received_content.end(), p.data().begin(), p.data().end());
        }


        if (!p.is_continued())
        {
            // Last packet
            sock->stop();

            std::stringstream ss;
            for (auto c : received_content)
            {
                ss << static_cast<char>(c);
            }
            std::string s{ss.str()};

            // Don't compare the actual signature as it changes when Debian releases a new version.
            bool has_begining = s.find("-----BEGIN PGP SIGNATURE-----") != std::string::npos;
            bool has_ending = s.find("-----END PGP SIGNATURE-----") != std::string::npos;
            assert(has_begining);
            assert(has_ending);
            Log::info("Result", "Data received!");
        }
    }

    void App::event(const ConnectionStatusEvent& ev)
    {
        Log::info("Connection status: ", Format("{1}", Bool(ev.is_connected())));

        sock->send(
                HTTPPacket(HTTPPacket::Method::GET, "/debian-cd/current-live/amd64/iso-hybrid/MD5SUMS.sign",
                           {
                                   {"UserAgent", "Mozilla/4.0"},
                                   {"Host",      "tp.sunet.se"}
                           }));
    }
}

