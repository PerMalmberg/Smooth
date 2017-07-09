//
// Created by permal on 7/1/17.
//

#pragma once

#include <cstring>
#include <sys/socket.h>
#include <esp_event.h>
#include <smooth/Task.h>
#include <map>
#include <vector>
#include <smooth/ipc/RecursiveMutex.h>
#include <smooth/ipc/TaskEventQueue.h>
#include "ISocket.h"

namespace smooth
{
    namespace network
    {

        class SocketDispatcher
                : public smooth::Task,
                  public smooth::ipc::IEventListener<system_event_t>
        {
            public:

                virtual ~SocketDispatcher()
                {
                }

                static SocketDispatcher& instance();

                void tick() override;

                void add_socket(ISocket* socket);
                void socket_closed(ISocket* socket);

                void message(const system_event_t& msg) override;

            private:
                SocketDispatcher();
                int build_sets();
                void clear_sets();
                void set_timeout();
                void restart_inactive_sockets();

                std::map<int, ISocket*> active_sockets;
                std::vector<ISocket*> inactive_sockets;
                smooth::ipc::RecursiveMutex socket_guard;
                smooth::ipc::TaskEventQueue<system_event_t> system_events;

                fd_set read_set;
                fd_set write_set;
                timeval tv;
                bool has_ip = false;
        };
    }
}
