//
// Created by permal on 7/1/17.
//

#include <smooth/network/SocketWorker.h>
#include <fcntl.h>
#include <algorithm>

namespace smooth
{
    namespace network
    {
        SocketWorker& SocketWorker::instance()
        {
            static SocketWorker instance;

            // Start task on first use
            static bool initialized = false;
            if (!initialized)
            {
                initialized = true;
                instance.start();
            }
            return instance;
        }


        SocketWorker::SocketWorker()
                : Task("SocketWorker", 8192, 6, 1, std::chrono::milliseconds(0)),
                  active_sockets(),
                  inactive_sockets(),
                  socket_guard(),
                  system_events("SocketWorker", 10, *this, *this)
        {
            clear_sets();
        }


        void SocketWorker::tick()
        {
            restart_inactive_sockets();

            int max_file_descriptor = build_sets();

            if (max_file_descriptor >= 0)
            {
                set_timeout();
                int res = select(max_file_descriptor + 1, &read_set, &write_set, nullptr, &tv);

                if (res == -1)
                {
                    // Error
                    const char* error = strerror(errno);
                    ESP_LOGE("SocketWorker", "Error during select: %s", error);
                }
                else if (res > 0)
                {
                    for (int i = 0; i <= max_file_descriptor; ++i)
                    {
                        if (FD_ISSET(i, &read_set))
                        {
                            auto& ptr_ref = active_sockets.at(i);
                            ISocket* ptr = *(&ptr_ref);
                            ptr->readable();
                        }

                        if (FD_ISSET(i, &write_set))
                        {
                            auto& ptr_ref = active_sockets.at(i);
                            ISocket* ptr = *(&ptr_ref);
                            ptr->writable();
                        }

                    }
                }
            }
            else
            {
                // When there are now active sockets, select() returns immediately so
                // we must prevent the task from hoging the CPU.
                delay(std::chrono::seconds(1));
            }
        }

        void SocketWorker::set_timeout()
        {
            tv.tv_sec = 1;
            tv.tv_usec = 0;
        }

        void SocketWorker::clear_sets()
        {
            smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
            FD_ZERO(&read_set);
            FD_ZERO(&write_set);
        }

        int SocketWorker::build_sets()
        {
            clear_sets();

            smooth::ipc::RecursiveMutex::Lock lock(socket_guard);

            int max = -1;

            for (auto* s : active_sockets)
            {
                max = std::max(max, s->get_socket_id());
                if (s->has_data_to_transmit())
                {
                    FD_SET(s->get_socket_id(), &write_set);
                }
                FD_SET(s->get_socket_id(), &read_set);
            }

            return max;
        }

        void SocketWorker::add_socket(ISocket* socket)
        {
            if (has_ip)
            {
                {
                    smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
                    active_sockets.push_back(socket);
                    ESP_LOGV("SocketWorker", "Added active: %d", socket->get_socket_id());
                }
                socket->internal_start();
            }
            else
            {
                smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
                inactive_sockets.push_back(socket);
                ESP_LOGV("SocketWorker", "Added inactive: %d", socket->get_socket_id());
            }
        }

        void SocketWorker::socket_closed(ISocket* socket)
        {
            ESP_LOGV("SocketWorker", "Socket closed: %d", socket->get_socket_id());
            smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
            auto it = std::find(active_sockets.begin(), active_sockets.end(), socket);
            if (it != active_sockets.end())
            {
                active_sockets.erase(it);
            }
        }

        void SocketWorker::restart_inactive_sockets()
        {
            if (has_ip)
            {
                // Make a copy of the inactive set - it might be modified
                // during the call to socket->internal_start().
                std::vector<ISocket*> copy = inactive_sockets;

                for (auto* socket: copy)
                {
                    ESP_LOGV("SocketWorker", "Restarting %d", socket->get_socket_id());
                    smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
                    active_sockets.push_back(socket);
                    socket->internal_start();
                }

                // Remove all sockets that are now on the active list
                smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
                for (auto* socket : copy)
                {
                    auto it = std::find(inactive_sockets.begin(), inactive_sockets.end(), socket);
                    if (it != inactive_sockets.end())
                    {
                        inactive_sockets.erase(it);
                    }
                }
            }
        }

        void SocketWorker::message(const system_event_t& msg)
        {
            if (msg.event_id == SYSTEM_EVENT_STA_GOT_IP
                || msg.event_id == SYSTEM_EVENT_AP_STA_GOT_IP6)
            {
                has_ip = true;
                restart_inactive_sockets();
            }
            else if (msg.event_id == SYSTEM_EVENT_STA_DISCONNECTED)
            {
                // Close all sockets
                has_ip = false;
                smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
                for (auto* socket : active_sockets)
                {
                    inactive_sockets.push_back(socket);
                    socket->stop();
                }
                active_sockets.clear();
            }
        }
    }
}
