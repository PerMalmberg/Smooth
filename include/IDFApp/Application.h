//
// Created by permal on 6/24/17.
//

#pragma once
#include <vector>
#include <esp_log.h>

class SystemEventListener;

class Application {
    public:
        static Application& instance();
        Application( const Application&) = delete;

        void subscribe(SystemEventListener* listener);
        void unsubscribe(SystemEventListener* listener);
        void publish_system_event( system_event_t& event );
        void set_system_log_level(esp_log_level_t level) const;
    private:
        Application();
        static esp_err_t event_callback(void *ctx, system_event_t *event);
        std::vector<SystemEventListener*> event_listeners;
};

