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

#include <unordered_map>

namespace smooth::application::network::mqtt
{
    enum PacketType
    {
        Reserved = 0,
        CONNECT = 1,
        CONNACK = 2,
        PUBLISH = 3,
        PUBACK = 4,
        PUBREC = 5,
        PUBREL = 6,
        PUBCOMP = 7,
        SUBSCRIBE = 8,
        SUBACK = 9,
        UNSUBSCRIBE = 10,
        UNSUBACK = 11,
        PINGREQ = 12,
        PINGRESP = 13,
        DISCONNECT = 14,
        Reserved_2 = 15
    };

    class PacketTypeHash
    {
        public:
            template<typename T>
            std::size_t operator()(T t) const
            {
                return static_cast<std::size_t>(t);
            }
    };

    extern const std::unordered_map<PacketType, const char*, PacketTypeHash> packet_type_as_string;

    enum QoS
    {
        AT_MOST_ONCE = 0,
        AT_LEAST_ONCE = 1,
        EXACTLY_ONCE = 2
    };
}
