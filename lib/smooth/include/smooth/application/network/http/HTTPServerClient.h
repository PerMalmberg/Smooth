#pragma once

#include <smooth/core/network/ServerClient.h>
#include <smooth/application/network/http/HTTPProtocol.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                template<int MaxPacketSize>
                class HTTPServerClient
                        : public smooth::core::network::ServerClient<HTTPServerClient<MaxPacketSize>, HTTPProtocol<MaxPacketSize>>
                {
                    public:
                        explicit HTTPServerClient(smooth::core::Task& task,
                                                  smooth::core::network::ClientPool<HTTPServerClient>& pool)
                                : core::network::ServerClient<
                                HTTPServerClient<MaxPacketSize>,
                                smooth::application::network::http::HTTPProtocol<MaxPacketSize>>(task, pool)
                        {
                        }

                        void
                        event(const smooth::core::network::event::DataAvailableEvent<HTTPProtocol<MaxPacketSize>>& /*event*/) override;

                        void event(const smooth::core::network::event::TransmitBufferEmptyEvent& /*event*/) override;

                        void disconnected() override;

                        void connected() override;

                        void reset_client() override;

                        std::chrono::milliseconds get_send_timeout() override
                        {
                            return std::chrono::seconds{1};
                        }
                };

                template<int MaxPacketSize>
                void HTTPServerClient<MaxPacketSize>::event(
                        const core::network::event::DataAvailableEvent<HTTPProtocol<MaxPacketSize>>&)
                {

                }

                template<int MaxPacketSize>
                void
                HTTPServerClient<MaxPacketSize>::event(const smooth::core::network::event::TransmitBufferEmptyEvent&)
                {

                }

                template<int MaxPacketSize>
                void HTTPServerClient<MaxPacketSize>::disconnected()
                {

                }

                template<int MaxPacketSize>
                void HTTPServerClient<MaxPacketSize>::connected()
                {

                }

                template<int MaxPacketSize>
                void HTTPServerClient<MaxPacketSize>::reset_client()
                {

                }
            }
        }
    }
}