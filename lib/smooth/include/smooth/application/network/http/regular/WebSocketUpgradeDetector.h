/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2020 Per Malmberg (https://gitbub.com/PerMalmberg)

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

#include <string>
#include "smooth/application/network/http/regular/HTTPRequestHandler.h"

namespace smooth::application::network::http::regular
{
    template<typename WSServerType>
    class WebSocketUpgradeDetector : public HTTPRequestHandler
    {
        public:
            void request(IConnectionTimeoutModifier& timeout_modifier,
                         const std::string& url,
                         const std::vector<uint8_t>& content)
            {
                (void)url;
                (void)content;

                if (is_last())
                {
                    auto did_upgrade = false;

                    try
                    {
                        const auto& upgrade = headers().at(UPGRADE);
                        const auto& connection = headers().at(CONNECTION);
                        const auto version = headers().at(SEC_WEBSOCKET_VERSION);

                        if (string_util::iequals(upgrade, "websocket")
                            && string_util::icontains(connection, "upgrade")
                            && string_util::equals(version, "13"))
                        {
                            const auto& key = headers().at(SEC_WEBSOCKET_KEY);
                            const char* websocket_key_constant = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

                            const auto concat = string_util::trim(key) + websocket_key_constant;
                            auto hash = hash::sha1(reinterpret_cast<const uint8_t*>(concat.data()), concat.length());

                            auto reply_key = hash::base64::encode(
                                reinterpret_cast<const uint8_t*>(hash.data()),
                                hash.size());

                            auto res = std::make_unique<regular::responses::HeaderOnlyResponse>(
                                ResponseCode::SwitchingProtocols);

                            res->add_header(UPGRADE, "websocket");
                            res->add_header(CONNECTION, "upgrade");
                            res->add_header(SEC_WEBSOCKET_ACCEPT, reply_key);
                            response().reply(std::move(res), false);
                            did_upgrade = true;

                            // Remove socket receive timeouts
                            timeout_modifier.set_receive_timeout(std::chrono::milliseconds{ 0 });

                            // Finally change protocols.
                            response().upgrade_to_websocket<WSServerType>();
                        }
                    }
                    catch (std::exception& ex)
                    {
                        Log::warning("WebSocketUpgradeDetector", "Websocket upgrade request failed: {}", ex.what());
                    }

                    if (!did_upgrade)
                    {
                        auto res = std::make_unique<regular::responses::HeaderOnlyResponse>(ResponseCode::Bad_Request);
                        response().reply(std::move(res), false);
                    }
                }
            }
    };
}
