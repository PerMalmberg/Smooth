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
#include <smooth/application/hash/base64.h>
#include <smooth/core/util/string_util.h>
#include <smooth/core/Task.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/filesystem/File.h>
#include <smooth/core/filesystem/Fileinfo.h>
#include <smooth/core/filesystem/filesystem.h>
#include <smooth/core/filesystem/Path.h>
#include <smooth/core/network/InetAddress.h>
#include <smooth/core/network/ServerSocket.h>
#include <smooth/core/network/SecureServerSocket.h>
#include <smooth/application/network/http/http_utils.h>
#include <smooth/application/network/http/HTTPProtocol.h>
#include <smooth/application/network/http/HTTPServerClient.h>
#include <smooth/application/network/http/regular/responses/ErrorResponse.h>
#include <smooth/application/network/http/regular/responses/FileContentResponse.h>
#include <smooth/application/network/http/regular/TemplateProcessor.h>
#include <smooth/application/hash/sha.h>
#include "regular/RequestHandlerSignature.h"

namespace smooth::application::network::http
{
    // https://upload.wikimedia.org/wikipedia/commons/8/88/Http-headers-status.png

    class HTTPServerConfig
    {
        public:
            using DataRetriever = std::function<std::string(void)>;

            HTTPServerConfig(const HTTPServerConfig&) = default;

            HTTPServerConfig(HTTPServerConfig&&) noexcept = default;

            HTTPServerConfig& operator=(const HTTPServerConfig&) = delete;

            HTTPServerConfig& operator=(HTTPServerConfig&&) = delete;

            HTTPServerConfig(smooth::core::filesystem::Path web_root,
                             std::vector<std::string> index_files,
                             std::set<std::string> template_files,
                             std::shared_ptr<ITemplateDataRetriever> template_data_retriever,
                             std::size_t max_header_size,
                             std::size_t content_chunk_size)
                    : root_path(std::move(web_root)),
                      index(std::move(index_files)),
                      template_files(std::move(template_files)),
                      template_data_retriever(std::move(template_data_retriever)),
                      maximum_header_size(max_header_size),
                      content_chunk_size(content_chunk_size)
            {
            }

            [[nodiscard]] smooth::core::filesystem::Path web_root() const
            {
                return root_path;
            }

            [[nodiscard]] const std::vector<std::string>& indexes() const
            {
                return index;
            }

            [[nodiscard]] const std::set<std::string>& templates() const
            {
                return template_files;
            }

            [[nodiscard]] std::size_t max_header_size() const
            {
                return maximum_header_size;
            }

            [[nodiscard]] std::size_t chunck_size() const
            {
                return content_chunk_size;
            }

            [[nodiscard]] std::shared_ptr<ITemplateDataRetriever> data_retriever() const
            {
                return template_data_retriever;
            }

        private:
            smooth::core::filesystem::Path root_path{};
            std::vector<std::string> index{};
            std::set<std::string> template_files{};
            std::shared_ptr<ITemplateDataRetriever> template_data_retriever{};
            std::size_t maximum_header_size{};
            std::size_t content_chunk_size{};
    };

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
                                            config.chunck_size());
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
                                            config.chunck_size());

                server->set_client_context(this);
                server->start(std::move(bind_to));
            }

            void on(HTTPMethod method, const std::string& url,
                    const RequestHandlerSignature& handler);

            template<typename WServerType>
            void enable_websocket_on(const std::string& url);

        private:
            using HandlerByURL = std::unordered_map<std::string, RequestHandlerSignature>;
            using HandlerByMethod = std::unordered_map<HTTPMethod, HandlerByURL>;

            void handle(HTTPMethod method,
                        IServerResponse& response,
                        IConnectionTimeoutModifier& timeout_modifier,
                        const std::string& requested_url,
                        const std::unordered_map<std::string, std::string>& request_headers,
                        const std::unordered_map<std::string, std::string>& request_parameters,
                        const std::vector<uint8_t>& data,
                        MIMEParser& mime,
                        bool fist_part,
                        bool last_part) override;

            template<typename WSServerType>
            void websocket_upgrade_detector(IServerResponse& response,
                                            IConnectionTimeoutModifier& timeout_modifier,
                                            const std::string& url,
                                            bool first_part,
                                            bool last_part,
                                            const std::unordered_map<std::string, std::string>& headers,
                                            const std::unordered_map<std::string, std::string>& request_parameters,
                                            const std::vector<uint8_t>& content,
                                            MIMEParser& mime);

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
                                    const RequestHandlerSignature& handler)
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
        MIMEParser& mime,
        bool fist_part,
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
                (*response_handler).second(response,
                                           timeout_modifier,
                                           requested_url,
                                           fist_part,
                                           last_part,
                                           request_headers,
                                           request_parameters,
                                           data,
                                           mime);
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
        auto f = [&](IServerResponse& response,
                     IConnectionTimeoutModifier& timeout_modifier,
                     const std::string& url, bool first_part,
                     bool last_part,
                     const std::unordered_map<std::string, std::string>& headers,
                     const std::unordered_map<std::string, std::string>& request_parameters,
                     const std::vector<uint8_t>& content, MIMEParser& mime) {
                     websocket_upgrade_detector<WSServerType>(response,
                                                     timeout_modifier,
                                                     url,
                                                     first_part,
                                                     last_part,
                                                     headers,
                                                     request_parameters,
                                                     content,
                                                     mime);
                 };

        on(HTTPMethod::GET, url, f);
    }

    template<typename ServerType>
    template<typename WSServerType>
    void HTTPServer<ServerType>::websocket_upgrade_detector(IServerResponse& response,
                                                            IConnectionTimeoutModifier& timeout_modifier,
                                                            const std::string& url,
                                                            bool first_part,
                                                            bool last_part,
                                                            const std::unordered_map<std::string, std::string>& headers,
                                                            const std::unordered_map<std::string,
                                                                                     std::string>& request_parameters,
                                                            const std::vector<uint8_t>& content,
                                                            MIMEParser& mime)
    {
        (void)response;
        (void)url;
        (void)first_part;
        (void)request_parameters;
        (void)content;
        (void)mime;

        if (last_part)
        {
            auto did_upgrade = false;

            try
            {
                const auto& upgrade = headers.at(UPGRADE);
                const auto& connection = headers.at(CONNECTION);
                const auto version = headers.at(SEC_WEBSOCKET_VERSION);

                if (string_util::iequals(upgrade, "websocket")
                    && string_util::icontains(connection, "upgrade")
                    && string_util::equals(version, "13"))
                {
                    const auto& key = headers.at(SEC_WEBSOCKET_KEY);
                    const char* websocket_key_constant = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

                    const auto concat = string_util::trim(key) + websocket_key_constant;
                    auto hash = hash::sha1(reinterpret_cast<const uint8_t*>(concat.data()), concat.length());

                    auto reply_key = hash::base64::encode(reinterpret_cast<const uint8_t*>(hash.data()), hash.size());

                    auto res = std::make_unique<regular::responses::HeaderOnlyResponse>(
                            ResponseCode::SwitchingProtocols);

                    res->add_header(UPGRADE, "websocket");
                    res->add_header(CONNECTION, "upgrade");
                    res->add_header(SEC_WEBSOCKET_ACCEPT, reply_key);
                    response.reply(std::move(res), false);
                    did_upgrade = true;

                    // Remove socket receive timeouts
                    timeout_modifier.set_receive_timeout(std::chrono::milliseconds{ 0 });

                    // Finally change protocols.
                    response.upgrade_to_websocket<WSServerType>();
                }
            }
            catch (std::exception& ex)
            {
                Log::warning(tag, "Websocket upgrade request failed: {}", ex.what());
            }

            if (!did_upgrade)
            {
                auto res = std::make_unique<regular::responses::HeaderOnlyResponse>(ResponseCode::Bad_Request);
                response.reply(std::move(res), false);
            }
        }
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
