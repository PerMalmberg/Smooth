//
// Created by permal on 6/24/17.
//

#include <algorithm>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <smooth/Application.h>
#include <smooth/SystemEventListener.h>

namespace smooth
{

    Application::Application()
            : event_listeners()
    {
        nvs_flash_init();

        // Setup the system event callback so that we receive events.
        ESP_ERROR_CHECK(esp_event_loop_init(&Application::event_callback, this));
    }

    Application::~Application()
    {
        event_listeners.clear();
    }

    void Application::subscribe(SystemEventListener *listener)
    {
        event_listeners.push_back(listener);
    }

    void Application::unsubscribe(SystemEventListener *listener)
    {
        auto it = std::find(event_listeners.begin(), event_listeners.end(), listener);
        if (it != event_listeners.end())
        {
            event_listeners.erase(it);
        }
    }

    void Application::publish_system_event(system_event_t& event)
    {
        for (auto listener : event_listeners)
        {
            if (listener != nullptr)
            {
                listener->system_event(*this, event);
            }
        }
    }

    esp_err_t Application::event_callback(void *ctx, system_event_t *event)
    {
        Application *app = reinterpret_cast<Application *>( ctx );
        if (app != nullptr)
        {
            app->publish_system_event(*event);
        }
        ESP_LOGV("Application", "Got event: %d", event->event_id);

        return ESP_OK;
    }

    void Application::set_system_log_level(esp_log_level_t level) const
    {
        // Silence wifi logging
        std::vector<const char *> logs{"wifi", "tcpip_adapter"};
        for (auto log : logs)
        {
            esp_log_level_set(log, level);
        }
    }

}