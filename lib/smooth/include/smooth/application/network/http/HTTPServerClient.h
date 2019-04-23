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
                class HTTPServerClient
                        : public smooth::core::network::ServerClient<HTTPServerClient, HTTPProtocol>
                {
                    public:
                        explicit HTTPServerClient(smooth::core::Task& task,
                                                  smooth::core::network::ClientPool<HTTPServerClient>& pool)
                                : ServerClient<HTTPServerClient, smooth::application::network::http::HTTPProtocol>(task,
                                                                                                                   pool)
                        {
                        }

                        ~HTTPServerClient() override = default;

                        void event(const smooth::core::network::event::DataAvailableEvent<HTTPProtocol>& /*event*/) override
                        {

                        }

                        void event(const smooth::core::network::event::TransmitBufferEmptyEvent& /*event*/) override
                        {

                        }

                        void disconnected() override
                        {

                        }

                        void connected() override
                        {

                        }

                        void reset_client() override
                        {

                        }


                        std::chrono::milliseconds get_send_timeout() override
                        {
                            return std::chrono::seconds{1};
                        };

                };
            }
        }
    }
}