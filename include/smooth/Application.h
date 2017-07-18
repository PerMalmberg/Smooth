//
// Created by permal on 6/24/17.
//

#pragma once

#include <vector>
#include <chrono>
#include <esp_log.h>
#include <smooth/ipc/Queue.h>
#include <smooth/Task.h>
#include <smooth/network/Wifi.h>
#include <smooth/ipc/TaskEventQueue.h>

namespace smooth
{
    class Application
            : public Task,
              smooth::ipc::IEventListener<system_event_t>
    {
        public:
            Application(UBaseType_t priority, std::chrono::milliseconds tick_interval);

            virtual ~Application()
            {
            }

            Application(const Application&) = delete;

            void init() override;

            void set_system_log_level(esp_log_level_t level) const;

            void message( const system_event_t& msg) override;

            network::Wifi& get_wifi()
            {
                return wifi;
            }

        private:
            static esp_err_t event_callback(void* ctx, system_event_t* event);
            ipc::TaskEventQueue<system_event_t> system_event;
            network::Wifi wifi;
    };

}