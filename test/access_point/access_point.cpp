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
#include <smooth/core/logging/log.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/filesystem/filesystem.h>
#include <smooth/core/filesystem/FSLock.h>
#include <smooth/application/network/http/regular/responses/StringResponse.h>
#include <smooth/application/network/http/regular/MIMEParser.h>
#include <smooth/core/SystemStatistics.h>

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

        Log::info("App::Init", "Starting wifi...");
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

        std::string str_response = "";

        std::array<uint8_t, 6> mac;

        if (wifi.get_local_mac_address(mac))
        {
            Log::info("Tag", "loacal MAC: {:X}:{:X}:{:X}:{:X}:{:X}:{:X}", mac[0], mac[1], mac[2], mac[3], mac[4],
            mac[5]);
        }

        uint32_t ip = static_cast<uint32_t>(wifi.get_local_ip().addr);

        if (ip)
        {
            Log::info("Tag",
            "loacal IP: {}.{}.{}.{}",
            ip >> 24,
            (ip & 0x00FF0000) >> 16,
            (ip & 0x0000FF00) >> 8,
            ip & 0xFF);
        }

        std::string MAC = fmt::format("{:X}:{:X}:{:X}:{:X}:{:X}:{:X}", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        std::string IP =
            fmt::format("{}.{}.{}.{}", ip >> 24, (ip & 0x00FF0000) >> 16, (ip & 0x0000FF00) >> 8, ip & 0xFF);
        str_response = "<HTML><HEAD><TITLE>Hello World!</TITLE></HEAD><BODY><H1>Hello World!</H1>";
        str_response += "Access point IP is: " + IP + "<br>";
        str_response += "Access point MAC is:" + MAC + "</BODY></HTML>";

        auto hello_world = [str_response](
            IServerResponse& response,
            IConnectionTimeoutModifier& /*timeout_modifier*/,
            const std::string& /*url*/,
            bool /*first_part*/,
            bool last_part,
            const std::unordered_map<std::string, std::string>& /*headers*/,
            const std::unordered_map<std::string, std::string>& /*request_parameters*/,
            const std::vector<uint8_t>& /*content*/,
            MIMEParser& /*mime*/) {
                               if (last_part)
                               {
                                   response.reply(std::make_unique<responses::StringResponse>(
                                   ResponseCode::OK,
                                   str_response),
                                   false);
                               }
                           };

        // As there's no actual index file in this example, use a response handles instead when calling requesting "/".
        // See http_server_test to see how a web server with actual files is set up.
        insecure_server->on(HTTPMethod::GET, "/", hello_world);
    }
}
