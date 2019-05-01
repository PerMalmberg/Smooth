#include <utility>

#pragma once

#include <memory>
#include <functional>
#include "HTTPProtocol.h"
#include <smooth/core/Task.h>
#include <smooth/core/network/InetAddress.h>
#include <smooth/core/network/ServerSocket.h>
#include <smooth/core/network/SecureServerSocket.h>
#include <smooth/application/network/http/HTTPProtocol.h>
#include <smooth/application/network/http/HTTPServerClient.h>
#include "HTTPMethod.h"
#include "ResponseSignature.h"

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                // https://upload.wikimedia.org/wikipedia/commons/8/88/Http-headers-status.png

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                class HTTPServer
                        : private IRequestHandler<MaxHeaderSize, ContentChuckSize>
                {
                    public:
                        explicit HTTPServer(smooth::core::Task& task);

                        void start(std::shared_ptr<smooth::core::network::InetAddress> bind_to)
                        {
                            server = ServerType::create(task, 5);
                            server->set_client_context(this);
                            server->start(std::move(bind_to));
                        }

                        void start(std::shared_ptr<smooth::core::network::InetAddress> bind_to,
                                   const std::vector<unsigned char>& ca_chain,
                                   const std::vector<unsigned char>& own_cert,
                                   const std::vector<unsigned char>& private_key,
                                   const std::vector<unsigned char>& password)
                        {
                            server = ServerType::create(task, 5, ca_chain, own_cert, private_key, password);
                            server->set_client_context(this);
                            server->start(std::move(bind_to));
                        }


                        void on_post(const std::string&& url, const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler);

                    private:
                        void handle_post(core::network::IPacketSender<HTTPProtocol<MaxHeaderSize, ContentChuckSize>>& sender,
                                         const std::string& requested_url,
                                         const std::unordered_map<std::string, std::string>& request_headers,
                                         const std::unordered_map<std::string, std::string>& request_parameters,
                                         const std::vector<uint8_t>& data,
                                         bool fist_part,
                                         bool last_part) override;

                        smooth::core::Task& task;
                        std::shared_ptr<smooth::core::network::ServerSocket<
                                smooth::application::network::http::HTTPServerClient<MaxHeaderSize, ContentChuckSize>,
                                smooth::application::network::http::HTTPProtocol<MaxHeaderSize, ContentChuckSize>>> server{};
                        std::unordered_map<HTTPMethod, std::unordered_map<std::string, ResponseSignature<MaxHeaderSize, ContentChuckSize>>> handlers{};
                };


                template<typename ServerSocketType, int MaxHeaderSize, int ContentChuckSize>
                HTTPServer<ServerSocketType, MaxHeaderSize, ContentChuckSize>::HTTPServer(smooth::core::Task& task)
                        : task(task)
                {
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::on_post(const std::string&& url,
                                                                                      const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler)
                {
                    handlers[HTTPMethod::POST][url] = handler;
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void
                HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::handle_post(
                        core::network::IPacketSender<HTTPProtocol<MaxHeaderSize, ContentChuckSize>>& sender,
                        const std::string& requested_url,
                        const std::unordered_map<std::string, std::string>& request_headers,
                        const std::unordered_map<std::string, std::string>& request_parameters,
                        const std::vector<uint8_t>& data,
                        bool fist_part,
                        bool last_part)
                {
                    auto& post_responders = handlers[HTTPMethod::POST];
                    auto it = post_responders.find(requested_url);

                    if (it != post_responders.end())
                    {
                        (*it).second(sender, requested_url, fist_part, last_part, request_headers, request_parameters, data);
                    }
                }
            }
        }
    }
}



