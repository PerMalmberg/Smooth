//
// Created by permal on 6/24/17.
//

#include <algorithm>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <smooth/core/Application.h>
#include <smooth/core/ipc/Publisher.h>
#include <smooth/core//network/SocketDispatcher.h>

using namespace std::chrono;

namespace smooth
{
    namespace core
    {
        Application::Application(UBaseType_t priority, std::chrono::milliseconds tick_interval)
                : Task(xTaskGetCurrentTaskHandle(), priority, tick_interval),
                  system_event("system_event", 10, *this, *this)
        {
            nvs_flash_init();

            // Setup the system event callback so that we receive events.
            ESP_ERROR_CHECK(esp_event_loop_init(&Application::event_callback, this));
        }

        esp_err_t Application::event_callback(void* ctx, system_event_t* event)
        {
            // Publish event to listeners
            smooth::core::ipc::Publisher<system_event_t>::publish(*event);
            ESP_LOGV("Application", "Got event with id %d.", event->event_id);

            return ESP_OK;
        }

        void Application::set_system_log_level(esp_log_level_t level) const
        {
            // Silence wifi logging
            std::vector<const char*> logs{"wifi", "tcpip_adapter"};
            for (auto log : logs)
            {
                esp_log_level_set(log, level);
            }
        }

        void Application::init()
        {
            // Start socket dispatcher first of all so that it is
            // ready to receive network status events.
            network::SocketDispatcher::instance();

            if (wifi.is_configured())
            {
                wifi.connect_to_ap();
            }

        }

        void Application::message(const system_event_t& msg)
        {
            wifi.message(msg);
        }
    }
}