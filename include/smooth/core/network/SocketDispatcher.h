//
// Created by permal on 7/1/17.
//

#pragma once

#include <cstring>
#include <map>
#include <vector>
#include <mutex>
#include <sys/socket.h>
#include <smooth/core/Task.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include "ISocket.h"
#include <esp_event.h>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            /// The SocketDispatcher handles all tasks related to sockets and is responsible for
            /// creating and sending the necessary events to the application. As an application developer
            /// you should never have to care about this class.
            class SocketDispatcher
                    : public smooth::core::Task,
                      public smooth::core::ipc::IEventListener<system_event_t>
            {
                public:

                    virtual ~SocketDispatcher()
                    {
                    }

                    static SocketDispatcher& instance();

                    void tick() override;

                    void start_socket(std::shared_ptr<ISocket> socket);
                    void shutdown_socket(std::shared_ptr<ISocket> socket);

                    void event(const system_event_t& event) override;

                    void socket_created(std::shared_ptr<ISocket> socket);


                protected:
                private:
                    SocketDispatcher();
                    int build_sets();
                    void clear_sets();
                    void set_timeout();
                    void restart_inactive_sockets();

                    void complete_socket_shutdown();

                    void remove_socket_from_collection(std::vector<std::shared_ptr<ISocket>>& col,
                                                       std::shared_ptr<ISocket> socket);
                    void remove_socket_from_active_sockets(std::shared_ptr<ISocket>& socket);

                    std::map<int, std::shared_ptr<ISocket>> active_sockets;
                    std::vector<std::shared_ptr<ISocket>> inactive_sockets;
                    std::vector<std::shared_ptr<ISocket>> all_sockets;
                    std::vector<std::shared_ptr<ISocket>> sockets_to_close;
                    std::recursive_mutex socket_guard;
                    smooth::core::ipc::SubscribingTaskEventQueue<system_event_t> system_events;

                    fd_set read_set;
                    fd_set write_set;
                    timeval tv;
                    bool has_ip = false;
                    static constexpr const char* tag = "SocketDispatcher";
            };
        }
    }
}