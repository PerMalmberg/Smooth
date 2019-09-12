/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <algorithm>
#include <functional>
#include <smooth/core/network/SocketDispatcher.h>
#include <smooth/core/task_priorities.h>

#ifndef ESP_PLATFORM

#include <unistd.h>

#endif

#include <sys/socket.h>

using namespace smooth::core::logging;
using namespace std::chrono;

namespace smooth::core::network
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
            : Task(tag, 1024 * 20, SOCKET_DISPATCHER_PRIO, std::chrono::milliseconds(0)),
              active_sockets(),
              inactive_sockets(),
              socket_guard(),
              network_events(NetworkEventQueue::create(tag, 10, *this, *this)),
              socket_op(SocketOperationQueue::create("SocketOperations",

                                                     // TODO: When compiled for IDF, get proper value based on number of
                                                     // allowed sockets in sdkconfig
                                                     //  Note: If there are more than 20 sockets, this queue is too
                                                     // small.
                                                     20,
                                                     *this,
                                                     *this))
    {
        clear_sets();
    }

    void SocketDispatcher::tick()
    {
        std::lock_guard<std::mutex> lock(socket_guard);
        print_status();
        restart_inactive_sockets();
        check_socket_timeouts();

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
                    if (is_fd_set(static_cast<size_t>(i), read_set))
                    {
                        auto it = active_sockets.find(i);

                        if (it != active_sockets.end())
                        {
                            it->second->readable(*this);
                        }
                    }

                    if (is_fd_set(static_cast<size_t>(i), write_set))
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
            // Nothing to do, wait for work.
            // Note: We cannot block, waiting for a notification from a socket since we're the ones polling
            // the sockets to determine if there are work to be done. And since ESP-IDF does not guarantee
            // round-robin scheduling, std::this_thread::yield() is not an option as that results in this
            // thread hogging the CPU, starving other threads. As such, we must spend some time sleeping to
            // guarantee other tasks gets a chance to run.
            //
            // https://esp32.com/viewtopic.php?p=28594#p28589
            // https://docs.espressif.com/projects/esp-idf/en/v3.0.2/api-guides/freertos-smp.html#round-robin-scheduling
            //
            // Sleep times less than 1ms hogs the CPU due to the FreeRTOS tick interval.
            // In practice, this delay means that there is up to an additional 1ms delay for any socket
            // operation, but only when there was no socket read/write to do prior to that operation being queued.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void SocketDispatcher::set_timeout()
    {
        tv.tv_sec = 0;
        tv.tv_usec = 10000;
    }

    void SocketDispatcher::clear_sets()
    {
        FD_ZERO(&read_set);
        FD_ZERO(&write_set);
    }

    int SocketDispatcher::build_sets()
    {
        clear_sets();

        int max = ISocket::INVALID_SOCKET;

        for (auto& pair : active_sockets)
        {
            auto& s = pair.second;

            max = std::max(max, s->get_socket_id());

            if (s->is_active())
            {
                if (!is_backed_off(s->get_socket_id()))
                {
                    if (s->has_data_to_transmit() || !s->is_connected())
                    {
                        // A valid socket id is >= 0 so casting to unsigned is safe
                        set_fd(static_cast<size_t>(s->get_socket_id()), write_set);
                    }

                    if (s->is_connected())
                    {
                        // A valid socket id is >= 0 so casting to unsigned is safe
                        set_fd(static_cast<size_t>(s->get_socket_id()), read_set);
                    }
                }
            }
        }

        return max;
    }

    void SocketDispatcher::start_socket(const std::shared_ptr<ISocket>& socket)
    {
        std::lock_guard<std::mutex> lock(socket_guard);

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
        std::lock_guard<std::mutex> lock(socket_guard);

        Log::verbose(tag, Format("Shutting down socket {1}, ID: {2}",
                                 Pointer(socket.get()),
                                 Int32(socket->get_socket_id())));
        socket->stop_internal();
        remove_socket_from_active_sockets(socket);
        remove_socket_from_collection(inactive_sockets, socket);
        remove_backed_off_socket(socket->get_socket_id());

        auto socket_id = socket->get_socket_id();

        if (socket_id != ISocket::INVALID_SOCKET)
        {
            int res = shutdown(socket_id, SHUT_RDWR);

            // Don't log "Not connected" errors
            if (res < 0 && errno != ENOTCONN)
            {
                Log::error(tag, Format("Shutdown error: {1}", Str(strerror(errno))));
            }

            res = close(socket_id);

            if (res < 0)
            {
                Log::error(tag, Format("Close error: {1}", Str(strerror(errno))));
            }

            socket->clear_socket_id();
            socket->publish_connected_status();
        }

        if (socket->is_server())
        {
            // Server sockets needs to be automatically restarted;
            // they do not publish a connection status event.
            inactive_sockets.push_back(socket);
        }
    }

    void SocketDispatcher::remove_socket_from_collection(std::vector<std::shared_ptr<ISocket>>& col,
                                                         const std::shared_ptr<ISocket>& socket)
    {
        const auto predicate = [&socket](const std::shared_ptr<ISocket>& o) {
                                   return (o.get()) == (socket.get());
                               };

        const auto found = std::find_if(col.begin(), col.end(), predicate);

        if (found != col.end())
        {
            col.erase(found);
        }
    }

    void SocketDispatcher::remove_socket_from_active_sockets(std::shared_ptr<ISocket>& socket)
    {
        const auto predicate = [&socket](std::pair<int, const std::shared_ptr<ISocket>> o) {
                                   return o.second.get() == socket.get();
                               };

        const auto found = std::find_if(active_sockets.begin(), active_sockets.end(), predicate);

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
                    socket->stop("Socket dispatcher failed to start socket");
                }
            }

            inactive_sockets.clear();
        }
    }

    void SocketDispatcher::event(const NetworkStatus& event)
    {
        bool shall_close_sockets = false;

        std::lock_guard<std::mutex> lock(socket_guard);

        if (event.get_event() == NetworkEvent::GOT_IP)
        {
            Log::info(tag, Format(Str("Network up, sockets will be restarted.")));
            has_ip = true;
            shall_close_sockets = true;
        }
        else if (event.get_event() == NetworkEvent::DISCONNECTED)
        {
            Log::warning(tag, Format(Str("Network down, closing all sockets.")));

            // Close all sockets
            has_ip = false;
            shall_close_sockets = true;
        }

        if (shall_close_sockets)
        {
            std::for_each(active_sockets.begin(), active_sockets.end(),
                          [this](decltype(*active_sockets.begin())& s) {
                              this->perform_op(SocketOperation::Op::Stop, s.second);
                          });
        }
    }

    void SocketDispatcher::event(const SocketOperation& event)
    {
        if (event.get_op() == SocketOperation::Op::Start)
        {
            start_socket(event.get_socket());
        }
        else if (event.get_op() == SocketOperation::Op::AddActiveSocket)
        {
            auto socket = event.get_socket();
            active_sockets.insert(std::make_pair(socket->get_socket_id(), socket));
        }
        else
        {
            shutdown_socket(event.get_socket());
        }
    }

    void SocketDispatcher::perform_op(SocketOperation::Op op, std::shared_ptr<ISocket> socket)
    {
        socket_op->push(SocketOperation(op, std::move(socket)));
    }

    void SocketDispatcher::check_socket_timeouts()
    {
        for (auto& pair : active_sockets)
        {
            if (pair.second->has_send_expired())
            {
                Log::warning(tag, Format("Send timeout on socket {1} ({2} ms)", Pointer(pair.second.get()),
                                         Int64(pair.second->get_send_timeout().count())));
                pair.second->stop("Send timeout");
            }
            else if (pair.second->has_receive_expired())
            {
                Log::warning(tag, Format("Receive timeout on socket {1} ({2} ms)", Pointer(pair.second.get()),
                                         Int64(pair.second->get_receive_timeout().count())));
                pair.second->stop("Receive timeout");
            }
        }
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

    void SocketDispatcher::set_fd(FD socket_id, fd_set& fd)
    {
        FD_SET(socket_id, &fd);
    }

    bool SocketDispatcher::is_fd_set(FD socket_id, fd_set& fd)
    {
        return FD_ISSET(socket_id, &fd);
    }

    void SocketDispatcher::print_status() const
    {
        static steady_clock::time_point last = steady_clock::now();
        auto now = steady_clock::now();

        if (now - last > seconds{ 15 })
        {
            last = now;
            Log::info(tag, Format("Active sockets: {1}", UInt64(active_sockets.size())));
        }
    }

    void SocketDispatcher::back_off(int socket_id, std::chrono::milliseconds duration)
    {
        backed_off[socket_id] = steady_clock::now() + duration;
    }

    bool SocketDispatcher::is_backed_off(int socket_id)
    {
        const auto& it = backed_off.find(socket_id);
        bool b_off = it != backed_off.end();

        if (b_off)
        {
            const auto& pair = *it;

            if (pair.second < steady_clock::now())
            {
                // No longer backed off
                backed_off.erase(socket_id);
                b_off = false;
            }
        }

        return b_off;
    }

    void SocketDispatcher::remove_backed_off_socket(int socket_id)
    {
        const auto& it = backed_off.find(socket_id);

        if (it != backed_off.end())
        {
            backed_off.erase(it);
        }
    }

#pragma GCC diagnostic pop
}
