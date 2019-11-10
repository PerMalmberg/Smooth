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

#pragma once

#include <functional>
#include <memory>
#include "smooth/core/Application.h"
#include "smooth/application/network/http/HTTPServer.h"
#include "smooth/core/network/ServerSocket.h"
#include "smooth/core/network/SecureServerSocket.h"
#include "smooth/application/network/http/HTTPServerClient.h"
#include "smooth/application/network/http/HTTPProtocol.h"
#include "smooth/application/network/http/regular/ITemplateDataRetriever.h"

#ifdef ESP_PLATFORM
#include "smooth/core/filesystem/MMCSDCard.h"
#endif

namespace http_server_test
{
    class DataRetriever
        : public smooth::application::network::http::ITemplateDataRetriever
    {
        public:
            std::string get(const std::string& key) const override
            {
                std::string res;

                try
                {
                    res = data.at(key);
                }
                catch (std::out_of_range&)
                {
                }

                return res;
            }

            void add(const std::string&& key, std::string&& value)
            {
                data.emplace(key, value);
            }

        private:
            std::unordered_map<std::string, std::string> data{};
    };

    class App
        : public smooth::core::Application
    {
        public:
            App();

            void init() override;

            void tick() override;

        private:
#ifdef ESP_PLATFORM
            std::unique_ptr<smooth::core::filesystem::MMCSDCard> sd_card{};
#endif

            static constexpr int MaxHeaderSize = 1024;
            static constexpr int ContentChunkSize = 4096;
            static constexpr int MaxResponses = 10;
            using Client = smooth::application::network::http::HTTPServerClient;
            using Protocol = smooth::application::network::http::HTTPProtocol;

            std::unique_ptr<smooth::application::network::http::InsecureServer> insecure_server{};
            std::unique_ptr<smooth::application::network::http::SecureServer> secure_server{};
    };
}
