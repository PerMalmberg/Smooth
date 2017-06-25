//
// Created by permal on 6/24/17.
//

#include <esp_event.h>
#include <esp_log.h>
#include "Application.h"

Application::Application()
{
    esp_event_loop_init(&Application::event_callback, this);
}

esp_err_t Application::event_callback(void *ctx, system_event_t *event) {
    Application* app = (Application*)ctx;
    if( app != nullptr )
    {
        ESP_LOGV( "Application", "Got event: %d", event->event_id);
    }

    return ESP_OK;
}
