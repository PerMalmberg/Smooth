//
// Created by permal on 6/24/17.
//

#pragma once
#include <vector>
#include <esp_event.h>
#include <esp_event_loop.h>

class SystemEventListener;

class Application {
    public:
        static Application& instance();
        Application( const Application&) = delete;

        void subscribe(SystemEventListener* listener);
        void unsubscribe(SystemEventListener* listener);
        void publish_system_event( system_event_t& event );
    private:
        Application();
        static esp_err_t event_callback(void *ctx, system_event_t *event);
        std::vector<SystemEventListener*> event_listeners;
};

