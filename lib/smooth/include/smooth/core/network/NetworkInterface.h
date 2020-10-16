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
#include <esp_netif.h>
#include <string>

namespace smooth::core::network {
/// Ethernet management class
class NetworkInterface {
public:
    NetworkInterface(std::string name);

    NetworkInterface(const NetworkInterface&) = delete;

    NetworkInterface(NetworkInterface&&) = delete;

    NetworkInterface& operator=(const NetworkInterface&) = delete;

    NetworkInterface& operator=(NetworkInterface&&) = delete;

    virtual ~NetworkInterface();

    /// Sets the hostname
    /// \param name The name
    void set_host_name(const std::string& name);

    /// Returns a value indicating of currently connected to the access point.
    /// \return
    [[nodiscard]] bool is_connected() const;

    [[nodiscard]] std::string get_mac_address() const;

    [[nodiscard]] uint32_t get_local_ip() const;

    bool get_local_mac_address(std::array<uint8_t, 6>& m) const;

    void close_if();

protected:
    esp_ip4_addr ip = {0};
    bool connected = false;
    esp_netif_t* interface{nullptr};
    static uint8_t interface_count;
    std::string interface_name;
};
}
