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
#include <smooth/application/network/http/responses/Response.h>
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

                        void start(int max_client_count, std::shared_ptr<smooth::core::network::InetAddress> bind_to)
                        {
                            server = ServerType::create(task, max_client_count);
                            server->set_client_context(this);
                            server->start(std::move(bind_to));
                        }

                        void start(int max_client_count, std::shared_ptr<smooth::core::network::InetAddress> bind_to,
                                   const std::vector<unsigned char>& ca_chain,
                                   const std::vector<unsigned char>& own_cert,
                                   const std::vector<unsigned char>& private_key,
                                   const std::vector<unsigned char>& password)
                        {
                            server = ServerType::create(task, max_client_count, ca_chain, own_cert, private_key,
                                                        password);
                            server->set_client_context(this);
                            server->start(std::move(bind_to));
                        }


                        void on_post(const std::string&& url,
                                     const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler);

                        void on_get(const std::string&& url,
                                    const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler);

                        void on_put(const std::string&& url,
                                    const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler);

                        void on_head(const std::string&& url,
                                     const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler);

                        void on_delete(const std::string&& url,
                                       const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler);

                    private:
                        using MethodHandler = std::unordered_map<std::string, ResponseSignature<MaxHeaderSize, ContentChuckSize>>;

                        void handle_post(IResponseQueue& response,
                                         const std::string& requested_url,
                                         const std::unordered_map<std::string, std::string>& request_headers,
                                         const std::unordered_map<std::string, std::string>& request_parameters,
                                         const std::vector<uint8_t>& data,
                                         bool fist_part,
                                         bool last_part) override;

                        void handle_get(IResponseQueue& response,
                                        const std::string& requested_url,
                                        const std::unordered_map<std::string, std::string>& request_headers,
                                        const std::unordered_map<std::string, std::string>& request_parameters,
                                        const std::vector<uint8_t>& data,
                                        bool fist_part,
                                        bool last_part) override;

                        void handle_put(IResponseQueue& response,
                                        const std::string& requested_url,
                                        const std::unordered_map<std::string, std::string>& request_headers,
                                        const std::unordered_map<std::string, std::string>& request_parameters,
                                        const std::vector<uint8_t>& data,
                                        bool fist_part,
                                        bool last_part) override;

                        void handle_delete(IResponseQueue& response,
                                           const std::string& requested_url,
                                           const std::unordered_map<std::string, std::string>& request_headers,
                                           const std::unordered_map<std::string, std::string>& request_parameters,
                                           const std::vector<uint8_t>& data,
                                           bool fist_part,
                                           bool last_part) override;

                        void handle_head(IResponseQueue& response,
                                         const std::string& requested_url,
                                         const std::unordered_map<std::string, std::string>& request_headers,
                                         const std::unordered_map<std::string, std::string>& request_parameters,
                                         const std::vector<uint8_t>& data,
                                         bool fist_part,
                                         bool last_part) override;

                        void call_response_handler(MethodHandler& method_handlers,
                                                   IResponseQueue& response,
                                                   const std::string& requested_url,
                                                   const std::unordered_map<std::string, std::string>& request_headers,
                                                   const std::unordered_map<std::string, std::string>& request_parameters,
                                                   const std::vector<uint8_t>& data,
                                                   bool fist_part,
                                                   bool last_part);

                        smooth::core::Task& task;
                        std::shared_ptr<smooth::core::network::ServerSocket<
                                smooth::application::network::http::HTTPServerClient<MaxHeaderSize, ContentChuckSize>,
                                smooth::application::network::http::HTTPProtocol<MaxHeaderSize, ContentChuckSize>>> server{};

                        std::unordered_map<HTTPMethod, MethodHandler> handlers{};
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
                void HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::on_get(const std::string&& url,
                                                                                     const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler)
                {
                    handlers[HTTPMethod::GET][url] = handler;
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::on_put(const std::string&& url,
                                                                                     const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler)
                {
                    handlers[HTTPMethod::PUT][url] = handler;
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::on_head(const std::string&& url,
                                                                                      const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler)
                {
                    handlers[HTTPMethod::HEAD][url] = handler;
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::on_delete(const std::string&& url,
                                                                                        const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler)
                {
                    handlers[HTTPMethod::DELETE][url] = handler;
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::call_response_handler(
                        MethodHandler& method_handlers,
                        IResponseQueue& response,
                        const std::string& requested_url,
                        const std::unordered_map<std::string, std::string>& request_headers,
                        const std::unordered_map<std::string, std::string>& request_parameters,
                        const std::vector<uint8_t>& data,
                        bool fist_part,
                        bool last_part)
                {
                    auto handler = method_handlers.find(requested_url);
                    if (handler == method_handlers.end())
                    {
                        // No handler for this URL.
                        response.enqueue(std::make_unique<responses::Response>(ResponseCode::Not_Found));
                    }
                    else
                    {
                        (*handler).second(response,
                                          requested_url,
                                          fist_part,
                                          last_part,
                                          request_headers,
                                          request_parameters,
                                          data);
                    }
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void
                HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::handle_post(
                        IResponseQueue& response,
                        const std::string& requested_url,
                        const std::unordered_map<std::string, std::string>& request_headers,
                        const std::unordered_map<std::string, std::string>& request_parameters,
                        const std::vector<uint8_t>& data,
                        bool fist_part,
                        bool last_part)
                {
                    call_response_handler(handlers[HTTPMethod::POST],
                                          response,
                                          requested_url,
                                          request_headers,
                                          request_parameters,
                                          data,
                                          fist_part,
                                          last_part);
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void
                HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::handle_get(
                        IResponseQueue& response,
                        const std::string& requested_url,
                        const std::unordered_map<std::string, std::string>& request_headers,
                        const std::unordered_map<std::string, std::string>& request_parameters,
                        const std::vector<uint8_t>& data,
                        bool fist_part,
                        bool last_part)
                {
                    call_response_handler(handlers[HTTPMethod::GET],
                                          response,
                                          requested_url,
                                          request_headers,
                                          request_parameters,
                                          data,
                                          fist_part,
                                          last_part);
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void
                HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::handle_put(
                        IResponseQueue& response,
                        const std::string& requested_url,
                        const std::unordered_map<std::string, std::string>& request_headers,
                        const std::unordered_map<std::string, std::string>& request_parameters,
                        const std::vector<uint8_t>& data,
                        bool fist_part,
                        bool last_part)
                {
                    call_response_handler(handlers[HTTPMethod::PUT],
                                          response, requested_url,
                                          request_headers,
                                          request_parameters,
                                          data,
                                          fist_part,
                                          last_part);
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void
                HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::handle_delete(
                        IResponseQueue& response,
                        const std::string& requested_url,
                        const std::unordered_map<std::string, std::string>& request_headers,
                        const std::unordered_map<std::string, std::string>& request_parameters,
                        const std::vector<uint8_t>& data,
                        bool fist_part,
                        bool last_part)
                {
                    call_response_handler(handlers[HTTPMethod::DELETE],
                                          response,
                                          requested_url,
                                          request_headers,
                                          request_parameters,
                                          data,
                                          fist_part,
                                          last_part);
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void
                HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::handle_head(
                        IResponseQueue& response,
                        const std::string& requested_url,
                        const std::unordered_map<std::string, std::string>& request_headers,
                        const std::unordered_map<std::string, std::string>& request_parameters,
                        const std::vector<uint8_t>& data,
                        bool fist_part,
                        bool last_part)
                {
                    call_response_handler(handlers[HTTPMethod::HEAD],
                                          response,
                                          requested_url,
                                          request_headers,
                                          request_parameters,
                                          data,
                                          fist_part,
                                          last_part);
                }
            }
        }
    }
}



