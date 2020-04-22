/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <memory>
#include <functional>
#include <set>
#include <unordered_map>
#include "HTTPProtocol.h"
#include "regular/HTTPMethod.h"
#include "smooth/application/hash/base64.h"
#include "smooth/core/util/string_util.h"
#include "smooth/core/Task.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/filesystem/File.h"
#include "smooth/core/filesystem/Fileinfo.h"
#include "smooth/core/filesystem/filesystem.h"
#include "smooth/core/filesystem/Path.h"
#include "smooth/core/network/InetAddress.h"
#include "smooth/core/network/ServerSocket.h"
#include "smooth/core/network/SecureServerSocket.h"
#include "smooth/application/network/http/http_utils.h"
#include "smooth/application/network/http/HTTPProtocol.h"
#include "smooth/application/network/http/HTTPServerClient.h"
#include "smooth/application/network/http/regular/responses/ErrorResponse.h"
#include "smooth/application/network/http/regular/responses/FileContentResponse.h"
#include "smooth/application/network/http/regular/TemplateProcessor.h"
#include "smooth/application/hash/sha.h"
#include "regular/RequestHandlerSignature.h"
#include "regular/HTTPRequestHandler.h"
#include "regular/WebSocketUpgradeDetector.h"
#include "HTTPServerConfig.h"

namespace smooth::application::network::http
{
    // https://upload.wikimedia.org/wikipedia/commons/8/88/Http-headers-status.png

    template<typename ServerType>
    class HTTPServer
        : private IRequestHandler
    {
        public:
            HTTPServer(smooth::core::Task& task, const HTTPServerConfig& configuration);

            void start(int max_client_count, int backlog, std::shared_ptr<smooth::core::network::InetAddress> bind_to)
            {
                server = ServerType::create(task,
                                            max_client_count,
                                            backlog,
                                            config.max_header_size(),
                                            config.chunk_size(),
                                            config.max_responses());
                server->set_client_context(this);
                server->start(std::move(bind_to));
            }

            void start(int max_client_count,
                       int backlog,
                       std::shared_ptr<smooth::core::network::InetAddress> bind_to,
                       const std::vector<unsigned char>& ca_chain,
                       const std::vector<unsigned char>& own_cert,
                       const std::vector<unsigned char>& private_key,
                       const std::vector<unsigned char>& password)
            {
                server = ServerType::create(task,
                                            max_client_count,
                                            backlog,
                                            ca_chain,
                                            own_cert,
                                            private_key,
                                            password,
                                            config.max_header_size(),
                                            config.chunk_size(),
                                            config.max_responses());

                server->set_client_context(this);
                server->start(std::move(bind_to));
            }

            /// Configure a request handler to handle a specific HTTP verb and path.
            /// Responders may be used for multiple URLS and/or methods, but must not be shared
            /// between different instances of an HTTP server since there is no guarantee in
            /// what order the data arrives to the handler. (If a handler is state-less, then this
            /// limitation does not apply.)
            void on(HTTPMethod method, const std::string& url,
                    const std::shared_ptr<smooth::application::network::http::regular::HTTPRequestHandler>& handler);

            template<typename WServerType>
            void enable_websocket_on(const std::string& url);

        private:
            using HandlerByURL = std::unordered_map<std::string,
                                                    std::shared_ptr<smooth::application::network::http::regular::HTTPRequestHandler>>;
            using HandlerByMethod = std::unordered_map<HTTPMethod, HandlerByURL>;

            void handle(HTTPMethod method,
                        IServerResponse& response,
                        IConnectionTimeoutModifier& timeout_modifier,
                        const std::string& requested_url,
                        const std::unordered_map<std::string, std::string>& request_headers,
                        const std::unordered_map<std::string, std::string>& request_parameters,
                        const std::vector<uint8_t>& data,
                        bool fist_part,
                        bool last_part) override;

            smooth::core::filesystem::Path find_index(const smooth::core::filesystem::Path& search_path) const;

            void
            reply_with(IServerResponse& response, std::unique_ptr<IResponseOperation> res);

            void serve_file(const HTTPMethod& method, IServerResponse& response, const std::string& requested_url,
                            const std::unordered_map<std::string, std::string>& request_headers);

            smooth::core::Task& task;
            std::shared_ptr<smooth::core::network::ServerSocket<
                                smooth::application::network::http::HTTPServerClient,
                                smooth::application::network::http::HTTPProtocol, IRequestHandler>> server{};

            HandlerByMethod handlers{};
            HTTPServerConfig config;
            const char* tag = "HTTPServer";
            TemplateProcessor template_processor;
    };

    template<typename ServerSocketType>
    HTTPServer<ServerSocketType>::HTTPServer(smooth::core::Task& task, const HTTPServerConfig& configuration)
            :
              task(task),
              config(configuration),
              template_processor(configuration.templates(), config.data_retriever())
    {
    }

    template<typename ServerType>
    void HTTPServer<ServerType>::on(HTTPMethod method,
                                    const std::string& url,
                                    const std::shared_ptr<smooth::application::network::http::regular::HTTPRequestHandler>& handler)
    {
        handlers[method][url] = handler;
    }

    template<typename ServerType>
    void HTTPServer<ServerType>::handle(
        HTTPMethod method,
        IServerResponse& response,
        IConnectionTimeoutModifier& timeout_modifier,
        const std::string& requested_url,
        const std::unordered_map<std::string, std::string>& request_headers,
        const std::unordered_map<std::string, std::string>& request_parameters,
        const std::vector<uint8_t>& data,
        bool first_part,
        bool last_part)
    {
        using namespace smooth::core::logging;

        // Is there a URL for the given method?
        auto by_url = handlers.find(method);

        if (by_url != handlers.end())
        {
            // Is there a specific handler for this URL?
            auto response_handler = (*by_url).second.find(requested_url);

            if (response_handler == (*by_url).second.end())
            {
                // No handler for this URL, does it match a file path beneath the web root?
                serve_file(method, response, requested_url, request_headers);
            }
            else
            {
                auto handler = (*response_handler).second;

                if (first_part)
                {
                    // Call order is important - must update call params before calling the rest of the methods in the
                    // inheriting class.
                    handler->update_call_params(first_part, last_part, response, request_headers, request_parameters);
                    handler->prepare_mime();
                    handler->start_of_request();
                }

                handler->request(timeout_modifier,
                                requested_url,
                                data);

                if (last_part)
                {
                    handler->end_of_request();
                }
            }
        }
        else
        {
            // No specific handler for this URL.
            serve_file(method, response, requested_url, request_headers);
        }
    }

    template<typename ServerType>
    void HTTPServer<ServerType>::serve_file(const HTTPMethod& method,
                                            IServerResponse& response,
                                            const std::string& requested_url,
                                            const std::unordered_map<std::string, std::string>& request_headers)
    {
        auto found = false;

        Log::info(tag, "Request: {}: '{}'", utils::http_method_to_string(method), requested_url);

        filesystem::Path search{ config.web_root() };
        search /= requested_url;

        if (config.web_root().is_parent_of(search) || config.web_root() == search)
        {
            filesystem::FileInfo info(search);

            if (info.is_regular_file())
            {
                // Attempt to process the file as a template.
                auto processed_template = template_processor.process_template(info.path());

                if (processed_template)
                {
                    reply_with(response, std::move(processed_template));
                }
                else
                {
                    // Not a template, simply serve the requested file
                    bool send_not_modified = false;
                    auto if_modified_since = request_headers.find("if-modified-since");

                    if (if_modified_since != request_headers.end())
                    {
                        auto since = utils::parse_http_time((*if_modified_since).second);

                        if (since >= info.last_modified_point())
                        {
                            send_not_modified = true;
                        }
                    }

                    if (send_not_modified)
                    {
                        reply_with(response,
                                   std::make_unique<responses::ErrorResponse>(ResponseCode::Not_Modified));
                    }
                    else
                    {
                        reply_with(response, std::make_unique<responses::FileContentResponse>(search));
                    }
                }

                found = true;
            }
            else if (info.is_directory())
            {
                auto index_path = find_index(search);

                if (!index_path.empty())
                {
                    auto processed_template = template_processor.process_template(index_path);

                    if (processed_template)
                    {
                        reply_with(response, std::move(processed_template));
                    }
                    else
                    {
                        reply_with(response, std::make_unique<responses::FileContentResponse>(index_path));
                    }

                    found = true;
                }
            }
        }

        if (!found)
        {
            reply_with(response, std::make_unique<responses::ErrorResponse>(ResponseCode::Not_Found));
        }
    }

    template<typename ServerType>
    smooth::core::filesystem::Path HTTPServer<ServerType>::find_index(
        const smooth::core::filesystem::Path& search_path) const
    {
        smooth::core::filesystem::Path found_index{};

        for (auto index = config.indexes().begin(); found_index.empty() && index != config.indexes().end(); ++index)
        {
            auto index_path = search_path / *index;

            if (config.web_root().is_parent_of(index_path))
            {
                core::filesystem::FileInfo index_info(search_path / *index);

                if (index_info.is_regular_file())
                {
                    found_index = search_path / *index;
                }
            }
        }

        return found_index;
    }

    template<typename ServerType>
    void HTTPServer<ServerType>::reply_with(IServerResponse& response,
                                            std::unique_ptr<IResponseOperation> res)
    {
        Log::info(tag, "Reply: {}", response_code_to_text.at(res->get_response_code()));
        response.reply(std::move(res), false);
    }

    template<typename ServerType>
    template<typename WSServerType>
    void HTTPServer<ServerType>::enable_websocket_on(const std::string& url)
    {
        auto detector = std::make_shared<regular::WebSocketUpgradeDetector<WSServerType>>();

        on(HTTPMethod::GET, url, detector);
    }

    using Client = smooth::application::network::http::HTTPServerClient;
    using Protocol = smooth::application::network::http::HTTPProtocol;

    using InsecureServer = smooth::application::network::http::HTTPServer<smooth::core::network::ServerSocket<Client,
                                                                                                              Protocol,
                                                                                                              smooth::application::network::http::IRequestHandler>>;
    using SecureServer = smooth::application::network::http::HTTPServer<smooth::core::network::SecureServerSocket<Client,
                                                                                                                  Protocol,
                                                                                                                  smooth::application::network::http::IRequestHandler>>;
}
