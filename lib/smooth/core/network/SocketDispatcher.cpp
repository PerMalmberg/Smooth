//
// Created by permal on 7/1/17.
//

#include <algorithm>
#include <functional>
#include <smooth/core/network/SocketDispatcher.h>
#include <smooth/core/task_priorities.h>

#ifndef ESP_PLATFORM

#include <unistd.h>

#endif

#include <sys/socket.h>

using namespace smooth::core::logging;

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
            : Task(tag, 1024 * 10, SOCKET_DISPATCHER_PRIO, std::chrono::milliseconds(0)),
              active_sockets(),
              inactive_sockets(),
              socket_guard(),
              network_events(tag, 10, *this, *this),
              socket_op("SocketOperations",
                      // Note: If there are more than 20 sockets, this queue is too small.
                        20,
                        *this,
                        *this)
    {
        clear_sets();
    }

    void SocketDispatcher::tick()
    {
        std::lock_guard<std::mutex> lock(socket_guard);
        restart_inactive_sockets();
        check_socket_send_timeout();

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
                            it->second->readable();
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

        int max = ISocket::INVALID_SOCKET;

        for (auto& pair : active_sockets)
        {
            auto& s = pair.second;

            max = std::max(max, s->get_socket_id());
            if (s->is_active())
            {
                if (s->has_data_to_transmit() || !s->is_connected())
                {
                    // A valid socket id is >= 0 so casting to unsigned is safe
                    set_fd(static_cast<size_t >(s->get_socket_id()), write_set);
                }

                if (s->is_connected())
                {
                    // A valid socket id is >= 0 so casting to unsigned is safe
                    set_fd(static_cast<size_t>(s->get_socket_id()), read_set);
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

        auto socket_id = socket->get_socket_id();
        if (socket_id != ISocket::INVALID_SOCKET)
        {
            int res = shutdown(socket_id, SHUT_RDWR);
            if (res < 0)
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
        const std::function<bool(const std::shared_ptr<ISocket>)> predicate = [socket](
                const std::shared_ptr<ISocket> o) {
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
                                  [socket](std::pair<int, std::shared_ptr<ISocket>> o) {
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

        std::lock_guard<std::mutex> lock(socket_guard);
        if (event.get_event() == NetworkEvent::GOT_IP)
        {
            Log::info(tag, Format(Str("Station got IP, sockets will be restarted.")));
            has_ip = true;
            shall_close_sockets = true;
        }
        else if (event.get_event() == NetworkEvent::DISCONNECTED)
        {
            Log::warning(tag, Format(Str("Station disconnected or IP lost, closing all sockets.")));

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
        socket_op.push(SocketOperation(op, std::move(socket)));
    }

    void SocketDispatcher::check_socket_send_timeout()
    {
        for (auto& pair : active_sockets)
        {
            if (pair.second->has_send_expired())
            {
                Log::verbose(tag, Format("Send timeout on socket {1}", Pointer(pair.second.get())));
                pair.second->stop();
            }
        }
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

    void SocketDispatcher::set_fd(std::size_t socket_id, fd_set& fd)
    {
        FD_SET(socket_id, &fd);
    }

    bool SocketDispatcher::is_fd_set(std::size_t socket_id, fd_set& fd)
    {
        return FD_ISSET(socket_id, &fd);
    }

#pragma GCC diagnostic pop
}