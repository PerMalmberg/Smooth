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

#pragma once

#include <functional>
#include <memory>
#include <smooth/core/Application.h>
#include <smooth/application/network/http/HTTPServer.h>
#include <smooth/core/network/ServerSocket.h>
#include <smooth/core/network/SecureServerSocket.h>
#include <smooth/application/network/http/HTTPServerClient.h>
#include <smooth/application/network/http/HTTPProtocol.h>

#ifdef ESP_PLATFORM
#include <smooth/core/filesystem/MMCSDCard.h>
#endif

namespace http_server_test
{
    class App
            : public smooth::core::Application
    {
        public:
            App();

            void init() override;

        private:
#ifdef ESP_PLATFORM
            std::unique_ptr<smooth::core::filesystem::MMCSDCard> sd_card{};
#endif

            static constexpr int MaxHeaderSize = 1024;
            static constexpr int ContentChunkSize = 4096;
            using Client = smooth::application::network::http::HTTPServerClient;
            using Protocol = smooth::application::network::http::HTTPProtocol;

            std::unique_ptr<smooth::application::network::http::HTTPServer<smooth::core::network::ServerSocket<Client, Protocol>>> insecure_server{};
            std::unique_ptr<smooth::application::network::http::HTTPServer<smooth::core::network::SecureServerSocket<Client, Protocol>>> secure_server{};
    };
}