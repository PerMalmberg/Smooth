//
// Created by permal on 6/24/17.
//

#include <algorithm>
#include <smooth/core/Application.h>
#include <smooth/core/ipc/Publisher.h>
#include <smooth/core/network/SocketDispatcher.h>

#ifdef ESP_PLATFORM
#include <freertos/portable.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>
#include <smooth/core/logging/log.h>
#endif // END ESP_PLATFORM

using namespace std::chrono;
using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        POSIXApplication::POSIXApplication(uint32_t priority, std::chrono::milliseconds tick_interval)
                : Task(priority, tick_interval)
        {
        }

        void POSIXApplication::init()
        {
            // Start socket dispatcher first of all so that it is
            // ready to receive network status events.
            network::SocketDispatcher::instance();
#ifndef ESP_PLATFORM
            // Assume network is available when running under POSIX system.
            network::NetworkStatus status(network::NetworkEvent::GOT_IP, true);
            core::ipc::Publisher<network::NetworkStatus>::publish(status);
#endif
        }

#ifdef ESP_PLATFORM
        const std::unordered_map<int, const char*> IDFApplication::id_to_system_event = {
                {SYSTEM_EVENT_WIFI_READY,          "ESP32 WiFi ready"},
                {SYSTEM_EVENT_SCAN_DONE,           "ESP32 finish scanning AP"},
                {SYSTEM_EVENT_STA_START,           "ESP32 station start"},
                {SYSTEM_EVENT_STA_STOP,            "ESP32 station stop"},
                {SYSTEM_EVENT_STA_CONNECTED,       "ESP32 station connected to AP"},
                {SYSTEM_EVENT_STA_DISCONNECTED,    "ESP32 station disconnected from AP"},
                {SYSTEM_EVENT_STA_AUTHMODE_CHANGE, "the auth mode of AP connected by ESP32 station changed"},
                {SYSTEM_EVENT_STA_GOT_IP,          "ESP32 station got IP from connected AP"},
                {SYSTEM_EVENT_STA_WPS_ER_SUCCESS,  "ESP32 station wps succeeds in enrollee mode"},
                {SYSTEM_EVENT_STA_WPS_ER_FAILED,   "ESP32 station wps fails in enrollee mode"},
                {SYSTEM_EVENT_STA_WPS_ER_TIMEOUT,  "ESP32 station wps timeout in enrollee mode"},
                {SYSTEM_EVENT_STA_WPS_ER_PIN,      "ESP32 station wps pin code in enrollee mode"},
                {SYSTEM_EVENT_AP_START,            "ESP32 soft-AP start"},
                {SYSTEM_EVENT_AP_STOP,             "ESP32 soft-AP stop"},
                {SYSTEM_EVENT_AP_STACONNECTED,     "a station connected to ESP32 soft-AP"},
                {SYSTEM_EVENT_AP_STADISCONNECTED,  "a station disconnected from ESP32 soft-AP"},
                {SYSTEM_EVENT_AP_PROBEREQRECVED,   "Receive probe request packet in soft-AP interface"},
                {SYSTEM_EVENT_AP_STA_GOT_IP6,      "ESP32 station or ap interface v6IP addr is preferred"},
                {SYSTEM_EVENT_ETH_START,           "ESP32 ethernet start"},
                {SYSTEM_EVENT_ETH_STOP,            "ESP32 ethernet stop"},
                {SYSTEM_EVENT_ETH_CONNECTED,       "ESP32 ethernet phy link up"},
                {SYSTEM_EVENT_ETH_DISCONNECTED,    "ESP32 ethernet phy link down"},
                {SYSTEM_EVENT_ETH_GOT_IP,          "ESP32 ethernet got IP from connected AP"},
                {SYSTEM_EVENT_MAX,                 ""}
        };

        static_assert(SYSTEM_EVENT_ETH_GOT_IP == SYSTEM_EVENT_MAX - 1, "System events have changed, id_to_system_event needs an update.");

        IDFApplication::IDFApplication(uint32_t priority, std::chrono::milliseconds tick_interval)
                : POSIXApplication(priority, tick_interval),
                  system_event("system_event", 10, *this, *this)
        {
            nvs_flash_init();
            gpio_install_isr_service(0);

            // Setup the system event callback so that we receive events.
            ESP_ERROR_CHECK(esp_event_loop_init(&IDFApplication::event_callback, this));
        }

        esp_err_t IDFApplication::event_callback(void* ctx, system_event_t* event)
        {

            auto name = id_to_system_event.find(event->event_id);
            if (name != id_to_system_event.end())
            {
                Log::verbose("Application", Format((*name).second));
            }
            else
            {
                Log::verbose("Application", Format("Got untranslated event id {1}", Int32(event->event_id)));
            }

            // Publish event to listeners
            smooth::core::ipc::Publisher<system_event_t>::publish(*event);

            return ESP_OK;
        }

        void IDFApplication::init()
        {
            // Start socket dispatcher first of all so that it is
            // ready to receive network status events.
            network::SocketDispatcher::instance();

            if (wifi.is_configured())
            {
                wifi.connect_to_ap();
            }
        }

        void IDFApplication::event(const system_event_t& event)
        {
            wifi.event(event);
        }
#endif
    }
}