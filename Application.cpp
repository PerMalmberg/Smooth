//
// Created by permal on 6/24/17.
//

#include <algorithm>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>
#include <tcpip_adapter.h>
#include <IDFApp/Application.h>
#include <IDFApp/SystemEventListener.h>

Application::Application()
        : event_listeners()
{
    nvs_flash_init();

    ESP_ERROR_CHECK(esp_event_loop_init(nullptr, nullptr));
    esp_event_loop_set_cb(&Application::event_callback, this);
}

Application& Application::instance()
{
    static Application app;
    return app;
}

void Application::subscribe(SystemEventListener* listener)
{
    event_listeners.push_back(listener);
}

void Application::unsubscribe(SystemEventListener* listener)
{
    auto it = std::find( event_listeners.begin(), event_listeners.end(), listener);
    if( it != event_listeners.end() )
    {
        event_listeners.erase(it);
    }
}

void Application::publish_system_event( system_event_t& event )
{
    for( auto listener : event_listeners)
    {
        if( listener != nullptr )
        {
            listener->system_event( *this, event );
        }
    }
}

esp_err_t Application::event_callback(void *ctx, system_event_t *event)
{
    esp_event_process_default(event);

    Application *app = reinterpret_cast<Application *>( ctx );
    if (app != nullptr)
    {
        app->publish_system_event( *event );
    }
    ESP_LOGV("Application", "Got event: %d", event->event_id);

    return ESP_OK;
}

void Application::set_system_log_level( esp_log_level_t level) const
{
    // Silence wifi logging
    std::vector<const char*> logs{ "wifi", "tcpip_adapter" };
    for( auto log : logs )
    {
        esp_log_level_set(log, level);
    }
}