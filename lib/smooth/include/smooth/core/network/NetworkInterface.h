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
#include <array>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <esp_netif.h>
#pragma GCC diagnostic pop
#include <string>

namespace smooth::core::network {

class NetworkInterface {
public:
    NetworkInterface(std::string&& name);

    NetworkInterface(const NetworkInterface&) = delete;

    NetworkInterface(NetworkInterface&&) = delete;

    NetworkInterface& operator=(const NetworkInterface&) = delete;

    NetworkInterface& operator=(NetworkInterface&&) = delete;

    virtual ~NetworkInterface();

    void set_host_name(const std::string& name);

    [[nodiscard]] bool is_connected() const;

    [[nodiscard]] std::string get_mac_address() const;

    [[nodiscard]] uint32_t get_local_ip() const;

    bool get_local_mac_address(std::array<uint8_t, 6>& m) const;

protected:
    void apply_host_name();

    esp_ip4_addr ip = {0};
    bool connected = false;
    esp_netif_t* interface{nullptr};
    static uint8_t interface_count;
    std::string interface_name;
    // esp_netif_set_hostname only copies a const char* pointer, so we need to keep this allocated here
    // max 32 characters
    char host_name[33] = "";
};
}
