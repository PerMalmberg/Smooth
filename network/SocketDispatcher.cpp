//
// Created by permal on 7/1/17.
//

#include <smooth/network/SocketDispatcher.h>
#include <fcntl.h>
#include <algorithm>

namespace smooth
{
    namespace network
    {
        SocketDispatcher& SocketDispatcher::instance()
        {
            static SocketDispatcher instance;

            // Start task on first use
            static bool initialized = false;
            if (!initialized)
            {
                initialized = true;
                instance.start();
            }
            return instance;
        }


        SocketDispatcher::SocketDispatcher()
                : Task("SocketDispatcher", 8192, 6, std::chrono::milliseconds(0)),
                  active_sockets(),
                  inactive_sockets(),
                  socket_guard(),
                  system_events("SocketDispatcher", 10, *this, *this)
        {
            clear_sets();
        }


        void SocketDispatcher::tick()
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
                    ESP_LOGE("SocketDispatcher", "Error during select: %s", error);
                }
                else if (res > 0)
                {
                    for (auto& it : active_sockets)
                    {
                        ESP_LOGV("SD", "ID: %d, %d", it.first, it.second->get_socket_id());
                    }


                    for (int i = 0; i <= max_file_descriptor; ++i)
                    {
                        if (FD_ISSET(i, &read_set))
                        {
                            auto it = active_sockets.find(i);
                            if (it != active_sockets.end())
                            {
                                it->second->readable();
                            }
                        }

                        // At this point, it is possible that a socket that was closed has been
                        // removed during the readable() call.
                        if (FD_ISSET(i, &write_set))
                        {
                            auto it = active_sockets.find(i);
                            if (it != active_sockets.end())
                            {
                                it->second->writable();
                            }
                        }
                    }
                }
            }
            else
            {
                // When there are no active sockets, select() returns immediately so
                // we must prevent the task from hogging the CPU, but we also want to
                // have short response time when something happens.
                delay(std::chrono::milliseconds(1));
            }
        }

        void SocketDispatcher::set_timeout()
        {
            tv.tv_sec = 1;
            tv.tv_usec = 0;
        }

        void SocketDispatcher::clear_sets()
        {
            smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
            FD_ZERO(&read_set);
            FD_ZERO(&write_set);
        }

        int SocketDispatcher::build_sets()
        {
            clear_sets();

            smooth::ipc::RecursiveMutex::Lock lock(socket_guard);

            int max = -1;

            for (auto& pair : active_sockets)
            {
                auto* s = pair.second;
                max = std::max(max, s->get_socket_id());
                if (s->has_data_to_transmit())
                {
                    FD_SET(s->get_socket_id(), &write_set);
                }
                FD_SET(s->get_socket_id(), &read_set);
            }

            return max;
        }

        void SocketDispatcher::add_socket(ISocket* socket)
        {
            if (has_ip)
            {
                {
                    smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
                    active_sockets.insert(std::make_pair(socket->get_socket_id(), socket));
                    ESP_LOGV("SocketDispatcher", "Added active: %p", socket);
                }
                socket->internal_start();
            }
            else
            {
                smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
                inactive_sockets.push_back(socket);
                ESP_LOGV("SocketDispatcher", "Added inactive: %p", socket);
            }
        }

        void SocketDispatcher::socket_closed(ISocket* socket)
        {
            ESP_LOGV("SocketDispatcher", "Socket closed: %p", socket);
            smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
            active_sockets.erase(socket->get_socket_id());
        }

        void SocketDispatcher::restart_inactive_sockets()
        {
            if (has_ip)
            {
                // Make a copy of the inactive set - it might be modified
                // during the call to socket->internal_start().
                std::vector<ISocket*> copy = inactive_sockets;

                for (auto* socket: copy)
                {
                    ESP_LOGV("SocketDispatcher", "Restarting %p", socket);
                    smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
                    active_sockets.insert(std::make_pair(socket->get_socket_id(), socket));
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

        void SocketDispatcher::message(const system_event_t& msg)
        {
            ESP_LOGV("SocketDispatcher", "Message: %d", msg.event_id);
            if (msg.event_id == SYSTEM_EVENT_STA_GOT_IP
                || msg.event_id == SYSTEM_EVENT_AP_STA_GOT_IP6)
            {
                has_ip = true;
            }
            else if (msg.event_id == SYSTEM_EVENT_STA_DISCONNECTED)
            {
                // Close all sockets
                has_ip = false;
                smooth::ipc::RecursiveMutex::Lock lock(socket_guard);
                for (auto& pair : active_sockets)
                {
                    inactive_sockets.push_back(pair.second);
                    pair.second->stop();
                }
                active_sockets.clear();
            }
        }
    }
}
