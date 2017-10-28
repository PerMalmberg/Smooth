//
// Created by permal on 7/1/17.
//

#include <smooth/core/network/SocketDispatcher.h>
#ifndef ESP_PLATFORM
#include <unistd.h>
#endif
#include <sys/socket.h>

using namespace smooth::core::logging;

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
                    : Task(tag, 8192, 10, std::chrono::milliseconds(0)),
                      active_sockets(),
                      inactive_sockets(),
                      socket_guard(),
                      network_events(tag, 10, *this, *this)
            {
                clear_sets();
            }

            void SocketDispatcher::tick()
            {
                std::lock_guard<std::recursive_mutex> lock(socket_guard);
                restart_inactive_sockets();

                int max_file_descriptor = build_sets();
                if (max_file_descriptor >= 0)
                {
                    set_timeout();
                    int res = select(max_file_descriptor + 1, &read_set, &write_set, nullptr, &tv);

                    if (res == -1)
                    {
                        Log::error(tag, Format("Error during select: {1}", Str(strerror(errno))));
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

                        if (s->is_connected())
                        {
                            FD_SET(s->get_socket_id(), &read_set);
                        }
                    }
                }

                return max;
            }

            void SocketDispatcher::start_socket(std::shared_ptr<ISocket> socket)
            {
                std::lock_guard<std::recursive_mutex> lock(socket_guard);

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
                std::lock_guard<std::recursive_mutex> lock(socket_guard);

                Log::verbose(tag, Format("Shutting down socket {1}", Pointer(socket.get())));
                socket->stop();
                remove_socket_from_active_sockets(socket);
                remove_socket_from_collection(inactive_sockets, socket);

                int res = shutdown(socket->get_socket_id(), SHUT_RDWR);
                if (res < 0)
                {
                    Log::error(tag, Format("Shutdown error: {1}", Str(strerror(errno))));
                }

                res = close(socket->get_socket_id());
                if (res < 0)
                {
                    Log::error(tag, Format("Close error: {1}", Str(strerror(errno))));
                }

                socket->publish_connected_status();
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

            void SocketDispatcher::event(const NetworkStatus& event)
            {
                bool shall_close_sockets = false;

                std::lock_guard<std::recursive_mutex> lock(socket_guard);
                if (event.event == NetworkEvent::GOT_IP )
                {
                    has_ip = true;
                    shall_close_sockets = event.ip_changed;
                }
                else if (event.event == NetworkEvent::DISCONNECTED)
                {
                    Log::warning(tag, Format(Str("Station disconnected or IP lost, closing all sockets.")));

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