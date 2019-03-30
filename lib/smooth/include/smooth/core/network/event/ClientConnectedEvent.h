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
                template<typename ProtocolClient>
                class ClientConnectedEvent
                {
                    public:
                        ClientConnectedEvent() = default;

                        ClientConnectedEvent(const ClientConnectedEvent&) = default;

                        explicit ClientConnectedEvent(std::shared_ptr<ProtocolClient> new_client)
                            : client(std::move(new_client))
                        {
                        }

                        const std::shared_ptr<ProtocolClient>get_client() const
                        {
                            return client;
                        }

                    private:
                        std::shared_ptr<ProtocolClient> client;
                };
            }
        }
    }
}