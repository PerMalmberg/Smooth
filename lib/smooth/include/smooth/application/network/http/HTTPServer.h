#include <utility>

#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include "HTTPProtocol.h"
#include <smooth/core/Task.h>
#include <smooth/core/filesystem/Path.h>
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
                        explicit HTTPServer(smooth::core::Task& task, smooth::core::filesystem::Path web_root);

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

                        void on(HTTPMethod method, const std::string& url,
                                const ResponseSignature<MaxHeaderSize, ContentChuckSize>& handler);

                    private:
                        using HandlerByURL = std::unordered_map<std::string, ResponseSignature<MaxHeaderSize, ContentChuckSize>>;
                        using HandlerByMethod = std::unordered_map<HTTPMethod, HandlerByURL>;

                        void handle(HTTPMethod method,
                                    IResponseQueue& response,
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

                        HandlerByMethod handlers{};
                        smooth::core::filesystem::Path root;
                };


                template<typename ServerSocketType, int MaxHeaderSize, int ContentChuckSize>
                HTTPServer<ServerSocketType, MaxHeaderSize, ContentChuckSize>::HTTPServer(smooth::core::Task& task,
                                                                                          smooth::core::filesystem::Path
                                                                                          web_root)
                        :
                        task(task),
                        root(std::move(web_root))
                {
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void
                HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::on(HTTPMethod method,
                                                                            const std::string& url,
                                                                            const ResponseSignature<MaxHeaderSize,
                                                                                    ContentChuckSize>& handler)
                {
                    handlers[method][url] = handler;
                }

                template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
                void
                HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::handle(
                        HTTPMethod method,
                        IResponseQueue& response,
                        const std::string& requested_url,
                        const std::unordered_map<std::string, std::string>& request_headers,
                        const std::unordered_map<std::string, std::string>& request_parameters,
                        const std::vector<uint8_t>& data,
                        bool fist_part,
                        bool last_part)
                {
                    // Is there any URLs for the given method?
                    auto by_url = handlers.find(method);

                    if (by_url != handlers.end())
                    {
                        // Is there a matching URL?
                        auto response_handler = (*by_url).second.find(requested_url);

                        if (response_handler == (*by_url).second.end())
                        {
                            // No handler for this URL.
                            response.enqueue(std::make_unique<responses::Response>(ResponseCode::Not_Found));
                        }
                        else
                        {
                            (*response_handler).second(response,
                                                       requested_url,
                                                       fist_part,
                                                       last_part,
                                                       request_headers,
                                                       request_parameters,
                                                       data);
                        }
                    }
                    else
                    {
                        // No handler for this method type
                        response.enqueue(std::make_unique<responses::Response>(ResponseCode::Method_Not_Allowed));
                    }
                }
            }
        }
    }
}



