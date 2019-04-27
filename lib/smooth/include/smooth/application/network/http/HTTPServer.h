#include <utility>

#pragma once

#include <memory>
#include "HTTPProtocol.h"
#include <smooth/core/Task.h>
#include <smooth/core/network/InetAddress.h>
#include <smooth/core/network/ServerSocket.h>
#include <smooth/core/network/SecureServerSocket.h>
#include <smooth/application/network/http/HTTPProtocol.h>
#include <smooth/application/network/http/HTTPServerClient.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                // https://upload.wikimedia.org/wikipedia/commons/8/88/Http-headers-status.png

                template<typename ServerType, int MaxPacketSize, int ContentChuckSize>
                class HTTPServer
                {
                    public:
                        explicit HTTPServer(smooth::core::Task& task);

                        void start(std::shared_ptr<smooth::core::network::InetAddress> bind_to)
                        {
                            server = ServerType::create(task, 5);
                            server->start(std::move(bind_to));
                        }

                        void start(std::shared_ptr<smooth::core::network::InetAddress> bind_to,
                                   const std::vector<unsigned char>& ca_chain,
                                   const std::vector<unsigned char>& own_cert,
                                   const std::vector<unsigned char>& private_key,
                                   const std::vector<unsigned char>& password)
                        {
                            server = ServerType::create(task, 5, ca_chain, own_cert, private_key, password);
                            server->start(std::move(bind_to));
                        }

                    private:
                        smooth::core::Task& task;
                        std::shared_ptr<smooth::core::network::ServerSocket<
                                                            smooth::application::network::http::HTTPServerClient<MaxPacketSize, ContentChuckSize>,
                                                            smooth::application::network::http::HTTPProtocol<MaxPacketSize, ContentChuckSize>>> server{};
                };

                template<typename ServerSocketType, int MaxPacketSize, int ContentChuckSize>
                HTTPServer<ServerSocketType, MaxPacketSize, ContentChuckSize>::HTTPServer(smooth::core::Task& task)
                        : task(task)
                {
                }
            }
        }
    }
}



