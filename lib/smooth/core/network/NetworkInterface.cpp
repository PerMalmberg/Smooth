#include "smooth/core/network/NetworkInterface.h"
#include "smooth/core/logging/log.h"
#include <sstream>

using namespace smooth::core::logging;

namespace smooth::core::network {
uint8_t NetworkInterface::interface_count = 0;

NetworkInterface::NetworkInterface(std::string name)
    : interface_name(name)
{
    if (interface_count == 0) {
        esp_netif_init();
    }
    ++interface_count;
}

NetworkInterface::~NetworkInterface()
{
    --interface_count;
    if (interface_count == 0) {
        esp_netif_deinit();
    }
};

void
NetworkInterface::set_host_name(const std::string& name)
{
    esp_netif_set_hostname(interface, name.c_str());
}

void
NetworkInterface::close_if()
{
    if (interface) {
        esp_netif_destroy(interface);
        interface = nullptr;
    }
}

// attention: access to this function might have a threading issue.
// It should be called from the main thread only!
uint32_t
NetworkInterface::get_local_ip() const
{
    return ip.addr;
}

bool
NetworkInterface::get_local_mac_address(std::array<uint8_t, 6>& m) const
{
    if (interface) {
        esp_err_t err = esp_netif_get_mac(interface, m.data());

        if (err != ESP_OK) {
            Log::error(interface_name, "get_local_mac_address(): {}", esp_err_to_name(err));
            return false;
        }
        return true;
    }

    return false;
}

std::string
NetworkInterface::get_mac_address() const
{
    std::stringstream mac;

    std::array<uint8_t, 6> m;
    bool ret = get_local_mac_address(m);

    if (ret) {
        for (const auto& v : m) {
            if (mac.tellp() > 0) {
                mac << "_";
            }

            mac << std::hex << static_cast<int>(v);
        }
    }

    return mac.str();
}
}