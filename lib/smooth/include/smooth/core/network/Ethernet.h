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
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <esp_eth.h>
#pragma GCC diagnostic pop
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/network/NetworkInterface.h"

namespace smooth::core::network {
/// Ethernet management class
class Ethernet : public NetworkInterface {
public:
    Ethernet(std::string name = "Ethernet");

    Ethernet(const Ethernet&) = delete;

    Ethernet(Ethernet&&) = delete;

    Ethernet& operator=(const Ethernet&) = delete;

    Ethernet& operator=(Ethernet&&) = delete;

    ~Ethernet();

    /// Calls do_init and logs errors that occur
    /// \return true on sucess
    bool init();
    void deinit();

    /// Returns a value indicating of currently connected to the access point.
    /// \return
    [[nodiscard]] bool is_connected() const;

    static void eth_event_callback(void* event_handler_arg,
                                   esp_event_base_t event_base,
                                   int32_t event_id,
                                   void* event_data);

private:
    void connect() const;

    esp_err_t do_init();

    void close_if();

    static void publish_status(bool connected, bool ip_changed);

    esp_event_handler_instance_t instance_eth_event{};
    esp_event_handler_instance_t instance_ip_event{};
    esp_netif_t* interface{nullptr};
    esp_eth_mac_t* mac{nullptr};
    esp_eth_phy_t* phy{nullptr};
    esp_eth_handle_t eth_handle{nullptr};
};
}
