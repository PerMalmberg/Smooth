//
// Created by permal on 7/1/17.
//

#include <fcntl.h>
#include <algorithm>
#include <smooth/core/network/SocketDispatcher.h>
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
                smooth::core::ipc::RecursiveMutex::Lock lock(socket_guard);
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
                FD_ZERO(&read_set);
                FD_ZERO(&write_set);
            }

            int SocketDispatcher::build_sets()
            {
                clear_sets();

                int max = -1;

                for (auto& pair : active_sockets)
                {
                    auto& s = pair.second;

                    max = std::max(max, s->get_socket_id());
                    if (s->is_active())
                    {
                        if (s->has_data_to_transmit() || !s->is_connected())
                        {
                            FD_SET(s->get_socket_id(), &write_set);
                        }
                        FD_SET(s->get_socket_id(), &read_set);
                    }
                }

                return max;
            }

            void SocketDispatcher::start_socket(std::shared_ptr<ISocket> socket)
            {
                smooth::core::ipc::RecursiveMutex::Lock lock(socket_guard);

                if (has_ip)
                {
                    if (socket->internal_start())
                    {
                        active_sockets.insert(std::make_pair(socket->get_socket_id(), socket));
                    }
                }
                else
                {
                    inactive_sockets.push_back(socket);
                }
            }

            void SocketDispatcher::shutdown_socket(std::shared_ptr<ISocket> socket)
            {
                smooth::core::ipc::RecursiveMutex::Lock lock(socket_guard);

                ESP_LOGV("SocketDispatcher", "Shutting down socket %p", socket.get());
                socket->stop();
                remove_socket_from_active_sockets(socket);
                remove_socket_from_collection(inactive_sockets, socket);

                int res = shutdown(socket->get_socket_id(), SHUT_RDWR);
                if (res < 0)
                {
                    ESP_LOGE("SocketDispatcher", "Shutdown error: %s", strerror(errno));
                }

                res = close(socket->get_socket_id());
                if (res < 0)
                {
                    ESP_LOGE("SocketDispatcher", "Close error: %s", strerror(errno));
                }

                socket->publish_connected_status(socket);
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
                    // Start and move sockets from inactive to active list
                    for (auto& socket : inactive_sockets)
                    {
                        if (socket->internal_start())
                        {
                            active_sockets.insert(std::make_pair(socket->get_socket_id(), socket));
                        }
                        else
                        {
                            socket->stop();
                        }
                    }

                    inactive_sockets.clear();
                }
            }

            void SocketDispatcher::event(const system_event_t& event)
            {
                bool shall_close_sockets = false;


                smooth::core::ipc::RecursiveMutex::Lock lock(socket_guard);
                if (event.event_id == SYSTEM_EVENT_STA_GOT_IP || event.event_id == SYSTEM_EVENT_AP_STA_GOT_IP6)
                {
                    has_ip = true;
                    shall_close_sockets = event.event_info.got_ip.ip_changed;
                }
                else if (event.event_id == SYSTEM_EVENT_STA_DISCONNECTED || event.event_id == SYSTEM_EVENT_STA_LOST_IP)
                {
                    ESP_LOGW("SocketDispatcher", "Station disconnected or IP lost, closing all sockets.");

                    // Close all sockets
                    has_ip = false;
                    shall_close_sockets = true;
                }

                if (shall_close_sockets)
                {
                    // shutdown_socket modifies active_sockets to we must work against a copy.
                    auto copy = active_sockets;
                    for (auto& socket : copy)
                    {
                        shutdown_socket(socket.second);
                    }
                }
            }
        }
    }
}