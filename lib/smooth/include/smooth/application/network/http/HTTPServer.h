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

                template<typename ServerType, int MaxPacketSize, int ContentChuckSize>
                class HTTPServer
                        : private IRequestHandler
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
                            server->set_client_context(this);
                            server->start(std::move(bind_to));
                        }


                        void on_post(const std::string&& url, const ResponseSignature& handler);

                    private:
                        void handle_post(const std::string& requested_url,
                                         const std::unordered_map<std::string, std::string>& request_headers,
                                         const std::unordered_map<std::string, std::string>& request_parameters,
                                         const std::vector<uint8_t>& data,
                                         bool fist_part,
                                         bool last_part) override;

                        smooth::core::Task& task;
                        std::shared_ptr<smooth::core::network::ServerSocket<
                                smooth::application::network::http::HTTPServerClient<MaxPacketSize, ContentChuckSize>,
                                smooth::application::network::http::HTTPProtocol<MaxPacketSize, ContentChuckSize>>> server{};
                        std::unordered_map<HTTPMethod, std::unordered_map<std::string, ResponseSignature>> handlers{};
                };


                template<typename ServerSocketType, int MaxPacketSize, int ContentChuckSize>
                HTTPServer<ServerSocketType, MaxPacketSize, ContentChuckSize>::HTTPServer(smooth::core::Task& task)
                        : task(task)
                {
                }

                template<typename ServerType, int MaxPacketSize, int ContentChuckSize>
                void HTTPServer<ServerType, MaxPacketSize, ContentChuckSize>::on_post(const std::string&& url,
                                                                                      const ResponseSignature& handler)
                {
                    handlers[HTTPMethod::POST][url] = handler;
                }

                template<typename ServerType, int MaxPacketSize, int ContentChuckSize>
                void
                HTTPServer<ServerType, MaxPacketSize, ContentChuckSize>::handle_post(const std::string& requested_url,
                                                                                     const std::unordered_map<std::string, std::string>& request_headers,
                                                                                     const std::unordered_map<std::string, std::string>& request_parameters,
                                                                                     const std::vector<uint8_t>& data,
                                                                                     bool fist_part,
                                                                                     bool last_part)
                {
                    auto& post_responders = handlers[HTTPMethod::POST];
                    auto it = post_responders.find(requested_url);

                    if(it != post_responders.end())
                    {
                        (*it).second(requested_url, fist_part, last_part, request_headers, request_parameters, data);


                    }
                }
            }
        }
    }
}



