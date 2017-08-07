//
// Created by permal on 7/1/17.
//

#include <smooth/core/network/SocketDispatcher.h>
#include <fcntl.h>
#include <algorithm>
#include "esp_log.h"

namespace smooth
{
    namespace core
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
                    : Task("SocketDispatcher", 8192, 6, std::chrono::milliseconds(100)),
                      active_sockets(),
                      inactive_sockets(),
                      all_sockets(),
                      sockets_to_close(),
                      socket_guard(),
                      system_events("SocketDispatcher", 10, *this, *this)
            {
                clear_sets();
            }

            void SocketDispatcher::tick()
            {
                smooth::core::ipc::RecursiveMutex::Lock lock(socket_guard);
                complete_socket_shutdown();
                restart_inactive_sockets();

                int max_file_descriptor = build_sets();

                if (max_file_descriptor >= 0)
                {
                    set_timeout();
                    int res = select(max_file_descriptor + 1, &read_set, &write_set, nullptr, &tv);

                    if (res == -1)
                    {
                        const char* error = strerror(errno);
                        ESP_LOGE("SocketDispatcher", "Error during select: %s", error);
                    }
                    else if (res > 0)
                    {
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
            }

            void SocketDispatcher::set_timeout()
            {
                tv.tv_sec = 0;
                tv.tv_usec = 1000;
            }

            void SocketDispatcher::clear_sets()
            {
                smooth::core::ipc::RecursiveMutex::Lock lock(socket_guard);
                FD_ZERO(&read_set);
                FD_ZERO(&write_set);
            }

            int SocketDispatcher::build_sets()
            {
                clear_sets();

                smooth::core::ipc::RecursiveMutex::Lock lock(socket_guard);

                int max = -1;

                for (auto& pair : active_sockets)
                {
                    auto& s = pair.second;

                    max = std::max(max, s->get_socket_id());
                    if (s->has_data_to_transmit())
                    {
                        FD_SET(s->get_socket_id(), &write_set);
                    }
                    FD_SET(s->get_socket_id(), &read_set);
                }

                return max;
            }

            void SocketDispatcher::start_socket(std::shared_ptr<ISocket> socket)
            {
                smooth::core::ipc::RecursiveMutex::Lock lock(socket_guard);

                if (has_ip)
                {
                    active_sockets.insert(std::make_pair(socket->get_socket_id(), socket));
                    socket->internal_start();
                }
                else
                {
                    inactive_sockets.push_back(socket);
                }
            }

            void SocketDispatcher::socket_created(std::shared_ptr<ISocket> socket)
            {
                all_sockets.push_back(socket);
            }

            void SocketDispatcher::initiate_shutdown(std::shared_ptr<ISocket> socket)
            {
                smooth::core::ipc::RecursiveMutex::Lock lock(socket_guard);

                // We can get this event SYSTEM_EVENT_STA_DISCONNECTED times so make sure not to put the sockets
                // in the list more than once. We're also calling Socket::stop() which in turn calls us so this
                // check has to be here.
                if (std::find(sockets_to_close.begin(), sockets_to_close.end(), socket) == sockets_to_close.end())
                {
                    ESP_LOGV("SocketDispatcher", "Initiating shutdown of socket %p", socket.get());
                    sockets_to_close.push_back(socket);
                    socket->stop();
                    socket->publish_connected_status(socket);
                }


            }

            void SocketDispatcher::complete_socket_shutdown()
            {
                for (auto& socket : sockets_to_close)
                {
                    ESP_LOGV("SocketDispatcher", "Completing shutdown of socket %p", socket.get());
                    remove_socket_from_active_sockets(socket);
                    remove_socket_from_collection(all_sockets, socket);
                    remove_socket_from_collection(inactive_sockets, socket);
                }

                sockets_to_close.clear();
            }

            void SocketDispatcher::remove_socket_from_collection(std::vector<std::shared_ptr<ISocket>>& col,
                                                                 std::shared_ptr<ISocket> socket)
            {
                const std::function<bool(const std::shared_ptr<ISocket>)> predicate = [socket](
                        const std::shared_ptr<ISocket> o)
                {
                    return (o.get()) == (socket.get());
                };

                auto found = std::find_if(col.begin(), col.end(), predicate);
                if (found != col.end())
                {
                    col.erase(found);
                }
            }

            void SocketDispatcher::remove_socket_from_active_sockets(std::shared_ptr<ISocket>& socket)
            {
                auto found = std::find_if(active_sockets.begin(), active_sockets.end(),
                                          [socket](std::pair<int, std::shared_ptr<ISocket>> o)
                                          {
                                              return o.second.get() == socket.get();
                                          });

                if (found != active_sockets.end())
                {
                    active_sockets.erase(found);
                }
            }

            void SocketDispatcher::restart_inactive_sockets()
            {
                if (has_ip)
                {
                    smooth::core::ipc::RecursiveMutex::Lock lock(socket_guard);

                    // Start and move sockets from inactive to active list
                    for (auto it = inactive_sockets.begin(); it != inactive_sockets.end();)
                    {
                        auto socket = *it;
                        socket->internal_start();
                        active_sockets.insert(std::make_pair(socket->get_socket_id(), socket));
                        it = inactive_sockets.erase(it);
                    }

                    // Monitor sockets that are started, but not yet connected
                    for (auto& pair : active_sockets)
                    {
                        auto socket = pair.second;
                        if (socket->is_active() && !socket->is_connected())
                        {
                            if (socket->check_if_connection_is_completed())
                            {
                                socket->publish_connected_status(socket);
                            }
                        }
                    }
                }
            }

            void SocketDispatcher::message(const system_event_t& msg)
            {
                smooth::core::ipc::RecursiveMutex::Lock lock(socket_guard);
                if (msg.event_id == SYSTEM_EVENT_STA_GOT_IP
                    || msg.event_id == SYSTEM_EVENT_AP_STA_GOT_IP6)
                {
                    has_ip = true;
                }
                else if (msg.event_id == SYSTEM_EVENT_STA_DISCONNECTED)
                {
                    // Close all sockets
                    has_ip = false;
                    for (auto& socket : all_sockets)
                    {
                        initiate_shutdown(socket);
                    }
                }
            }
        }
    }
}