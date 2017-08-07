//
// Created by permal on 6/24/17.
//

#pragma once

#include <vector>
#include <chrono>
#include <unordered_map>
#include <esp_log.h>
#include <smooth/core/ipc/Queue.h>
#include <smooth/core/Task.h>
#include <smooth/core/network/Wifi.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>

namespace smooth
{
    namespace core
    {
        /// The Application attaches itself to the main task and gives the application programmer
        /// the same possibilities to perform work on the main task as if a separate Task had been created.
        /// The Application class is also responsible for hooking the system event queue and distributing
        /// those events. Any application written based on Smooth should have an instance of the Application
        /// class (or a class derived from Application) on the stack in its app_main().
        /// Be sure to adjust the stack size of the main task accordingly using 'make menuconfig'.
        /// Note: Unlike the version of start() in Task, when called on an Application instance start() never returns.
        class Application
                : public Task,
                  smooth::core::ipc::IEventListener<system_event_t>
        {
            public:
                /// Constructor
                /// \param priority The priority to run at. Usually tskIDLE_PRIORITY + an arbitrary value,
                /// but should be lower than the priority of the ESP-IDFs task such as the Wifi driver.
                /// \param tick_interval The tick interval
                Application(UBaseType_t priority, std::chrono::milliseconds tick_interval);

                virtual ~Application()
                {
                }

                Application(const Application&) = delete;

                /// Initialize the application.
                void init() override;

                /// Convenience method to silence some system logs.
                void set_system_log_level(esp_log_level_t level) const;

                /// Event method for system events.
                /// \param event The event.
                void event(const system_event_t& event) override;

                /// Returns the Wifi manager
                /// \return The Wifi management instance
                network::Wifi& get_wifi()
                {
                    return wifi;
                }

            private:
                static esp_err_t event_callback(void* ctx, system_event_t* event);
                ipc::SubscribingTaskEventQueue<system_event_t> system_event;
                network::Wifi wifi;

                static const std::unordered_map<int, const char*> id_to_system_event;
        };
    }
}