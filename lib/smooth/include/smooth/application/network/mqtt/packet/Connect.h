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

#include <chrono>
#include <string>
#include "smooth/application/network/mqtt/packet/MQTTPacket.h"

namespace smooth::application::network::mqtt::packet
{
    class Connect
        : public MQTTPacket
    {
        public:
            Connect() = default;

            Connect(const std::string& client_id, std::chrono::seconds keep_alive, bool clean_session = true);

            bool get_clean_session();

        protected:
            bool has_payload() const override
            {
                return true;
            }

        private:
            bool clean_session{ true };
    };
}
