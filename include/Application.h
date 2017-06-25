//
// Created by permal on 6/24/17.
//

#pragma once

#include <esp_event.h>
#include <esp_event_loop.h>

class Application {
    public:
        Application();
        Application( const Application&) = delete;
    private:
        static esp_err_t event_callback(void *ctx, system_event_t *event);
};

