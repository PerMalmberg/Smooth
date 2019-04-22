#pragma once

#include "ISocket.h"

#ifndef ESP_PLATFORM

#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

#endif

namespace smooth
{
    namespace core
    {
        namespace network
        {
            class CommonSocket
                    : public ISocket,
                      public std::enable_shared_from_this<ISocket>
            {
                public:
                    void clear_socket_id() override;

                    int get_socket_id() const override;

                    void stop() override;

                    bool is_active() const override;

                    bool restart() override;

                protected:
                    bool set_non_blocking();
                    void log(const char* message);
                    void loge(const char* message);
                    bool is_connected() const override;

                    std::shared_ptr<InetAddress> ip{};
                    bool active = false;
                    bool connected = false;
                    int socket_id = INVALID_SOCKET;
            };
        }
    }
}