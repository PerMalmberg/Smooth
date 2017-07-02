//
// Created by permal on 7/1/17.
//

#pragma once

#include <cstring>
#include <sys/socket.h>
#include <esp_event.h>
#include <smooth/Task.h>
#include <map>
#include <smooth/ipc/Mutex.h>
#include <smooth/ipc/TaskEventQueue.h>

namespace smooth
{
    namespace network
    {
        class Socket;

        class SocketWorker
                : public smooth::Task,
                  public smooth::ipc::IEventListener<system_event_t>
        {
            public:

                virtual ~SocketWorker()
                {
                }

                static SocketWorker& instance();

                void tick() override;

                bool add_socket(int socket_id, smooth::network::Socket* socket);

                void message(const system_event_t& msg) override;

            private:
                SocketWorker();
                bool set_non_blocking(int socket_id);

                std::map<int, Socket*> sockets;
                smooth::ipc::Mutex fd_guard;
                smooth::ipc::TaskEventQueue<system_event_t> system_events;

                fd_set set;
                struct timeval tv;
        };
    }
}
