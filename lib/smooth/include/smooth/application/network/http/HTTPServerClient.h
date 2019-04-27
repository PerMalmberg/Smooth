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
                template<int MaxHeaderSize, int ContentChuckSize>
                class HTTPServerClient
                        : public smooth::core::network::ServerClient<HTTPServerClient<MaxHeaderSize, ContentChuckSize>, HTTPProtocol<MaxHeaderSize, ContentChuckSize>>
                {
                    public:
                        explicit HTTPServerClient(smooth::core::Task& task,
                                                  smooth::core::network::ClientPool<HTTPServerClient>& pool)
                                : core::network::ServerClient<
                                HTTPServerClient<MaxHeaderSize, ContentChuckSize>,
                                smooth::application::network::http::HTTPProtocol<MaxHeaderSize, ContentChuckSize>>(task, pool)
                        {
                        }

                        void
                        event(const smooth::core::network::event::DataAvailableEvent<HTTPProtocol<MaxHeaderSize, ContentChuckSize>>& /*event*/) override;

                        void event(const smooth::core::network::event::TransmitBufferEmptyEvent& /*event*/) override;

                        void disconnected() override;

                        void connected() override;

                        void reset_client() override;

                        std::chrono::milliseconds get_send_timeout() override
                        {
                            return std::chrono::seconds{1};
                        }
                };

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::event(
                        const core::network::event::DataAvailableEvent<HTTPProtocol<MaxHeaderSize, ContentChuckSize>>& event)
                {
                    typename HTTPProtocol<MaxHeaderSize, ContentChuckSize>::packet_type packet;
                    if(event.get(packet))
                    {

                    }
                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void
                HTTPServerClient<MaxHeaderSize, ContentChuckSize>::event(const smooth::core::network::event::TransmitBufferEmptyEvent&)
                {
                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::disconnected()
                {

                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::connected()
                {

                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::reset_client()
                {

                }
            }
        }
    }
}