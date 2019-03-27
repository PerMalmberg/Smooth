#include <utility>

#pragma once

#include <memory>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            namespace event
            {
                template<typename Protocol>
                class ClientConnectedEvent
                {
                    public:
                        ClientConnectedEvent() = default;

                        ClientConnectedEvent(const ClientConnectedEvent&) = default;

                        explicit ClientConnectedEvent(std::shared_ptr<Socket < Protocol>> client)
                            : socket (std::move(client))
                        {
                        }

                        const std::shared_ptr<Socket < Protocol>>get_socket() const
                        {
                            return socket;
                        }

                    private:
                        std::shared_ptr<Socket < Protocol>> socket;
                };
            }
        }
    }
}