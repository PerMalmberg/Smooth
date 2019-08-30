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

#include "access_point.h"
#include <memory>
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

        Log::info("App::Init", Format("Starting wifi..."));
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
                              ContentChunkSize };

        insecure_server = std::make_unique<InsecureServer>(*this, cfg);

        insecure_server->start(max_client_count, listen_backlog, std::make_shared<IPv4>("0.0.0.0", 8080));

        auto hello_world = [](
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
                                   "<HTML><HEAD><TITLE>Hello World!</TITLE></HEAD><BODY><H1>Hello World!</H1></BODY></HTML>"),
                                   false);
                               }
                           };

        // As there's no actual index file in this example, use a response handles instead when calling requesting "/".
        // See http_server_test to see how a web server with actual files is set up.
        insecure_server->on(HTTPMethod::GET, "/", hello_world);
    }
}
