//
// Created by permal on 7/1/17.
//

#include <smooth/network/SocketWorker.h>
#include <fcntl.h>

namespace smooth
{
    namespace network
    {
        SocketWorker& SocketWorker::instance()
        {
            static SocketWorker instance;
            return instance;
        }


        SocketWorker::SocketWorker()
                : Task("SocketWorker", 4096, 10, 1, std::chrono::milliseconds(0)),
                  sockets(),
                  fd_guard(),
                  system_events("SocketWorker", 10, *this, *this )
        {
            // Initialize socket sets
            FD_ZERO(&set);

            // Start the task
            start();
        }


        void SocketWorker::tick()
        {
            smooth::ipc::Mutex::Lock lock(fd_guard);
            //int res = select()
            delay(std::chrono::milliseconds(1));
        }

        bool SocketWorker::add_socket(int socket_id, smooth::network::Socket* socket)
        {
            smooth::ipc::Mutex::Lock lock(fd_guard);
            bool res = set_non_blocking(socket_id);
            if (res)
            {
                sockets.emplace(socket_id, socket);
                FD_SET(socket_id, &set);
            }

            return res;
        }

        bool SocketWorker::set_non_blocking(int socket_id)
        {
            bool res = true;

            auto opts = fcntl(socket_id, F_GETFL, 0);
            if (opts < 0)
            {
                ESP_LOGE("SocketWorker", "Could not get socket flags: %s", strerror(errno));
                res = false;
            }
            else if (fcntl(socket_id, F_SETFL, opts | O_NONBLOCK) < 0)
            {
                ESP_LOGE("SocketWorker", "Could not set socket flags: %s", strerror(errno));
                res = false;
            }

            return res;
        }


        void SocketWorker::message(const system_event_t& msg)
        {

        }
    }
}
