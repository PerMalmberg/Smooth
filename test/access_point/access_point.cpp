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

#include "access_point.h"
#include <memory>
#include <string>
#include <fmt/format.h>
#include "smooth/core/logging/log.h"
#include "smooth/core/task_priorities.h"
#include "smooth/core/network/IPv4.h"
#include "smooth/core/filesystem/filesystem.h"
#include "smooth/core/filesystem/FSLock.h"
#include "smooth/application/network/http/regular/responses/StringResponse.h"
#include "smooth/application/network/http/regular/HTTPRequestHandler.h"
#include "smooth/core/SystemStatistics.h"

#include "wifi_creds.h"

using namespace std;
using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::filesystem;
using namespace smooth::core::network;
using namespace smooth::core::network::event;
using namespace smooth::core::logging;
using namespace smooth::application::network::http;
using namespace smooth::application::network::http::responses;

namespace access_point
{
    // This request handler simply output a static web page, see http_server_test
    // on how to set up a web server with actual index files etc..
    class HelloWorldResponse : public smooth::application::network::http::regular::HTTPRequestHandler
    {
        public:
            constexpr static const char* tag = "HelloWorld";

            void request(IConnectionTimeoutModifier& /*timeout_modifier*/,
                         const std::string& /*url*/,
                         const std::vector<uint8_t>& /*content*/)
            {
                constexpr const char* mac_format = "Local MAC: {:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}";
                constexpr const char* ip_format = "Access Point IP: {}.{}.{}.{}";
                std::string mac_str{};
                std::string ip_str{};

                std::array<uint8_t, 6> mac{};

                if (Wifi::get_local_mac_address(mac))
                {
                    mac_str = fmt::format(mac_format, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                    Log::info(tag, mac_str);
                }
                else
                {
                    Log::error(tag, "Could not get local MAC");
                }

                auto ip = Wifi::get_local_ip();

                if (ip)
                {
                    ip_str = fmt::format(ip_format, ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
                    Log::info(tag, ip_str);
                }
                else
                {
                    Log::error(tag, "Local IP unavailable");
                }

                if (is_last())
                {
                    auto s = fmt::format(
                        "<HTML><HEAD><TITLE>Hello World!</TITLE></HEAD><BODY><H1>Hello World!</H1>"
                        "{}<br>{}</BODY></HTML>",
                        ip_str, mac_str);

                    response().reply(std::make_unique<responses::StringResponse>(
                                ResponseCode::OK,
                                s),
                                false);
                }
            }
    };

    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds(5000))
    {
    }

    void App::tick()
    {
        smooth::core::SystemStatistics::instance().dump();
    }

    void App::init()
    {
        std::stringstream ss;

        const int max_client_count = 6;
        const int listen_backlog = 6;

        Application::init();

        Log::info("App::init", "Starting wifi...");
        network::Wifi& wifi = get_wifi();
        wifi.set_host_name("Smooth-ESP");
        wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
        wifi.start_softap(2);

        Path web_root(FlashMount::instance().mount_point() / "web_root");

        flash = std::make_unique<smooth::core::filesystem::SPIFlash> (smooth::core::filesystem::FlashMount::instance(),
                                                     "app_storage",
                                                     10,
                                                     true);
        flash->mount();

        HTTPServerConfig cfg{ web_root, { "index.html" }, { ".html" }, nullptr, MaxHeaderSize,
                              ContentChunkSize, MaxResponses };

        insecure_server = std::make_unique<InsecureServer>(*this, cfg);

        insecure_server->start(max_client_count, listen_backlog, std::make_shared<IPv4>("0.0.0.0", 8080));

        insecure_server->on(HTTPMethod::GET, "/", std::make_shared<HelloWorldResponse>());
    }
}
