// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <functional>
#include <set>
#include <unordered_map>
#include "HTTPProtocol.h"
#include "regular/HTTPMethod.h"
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
                             const ITemplateDataRetriever& template_data_retriever,
                             std::size_t max_header_size,
                             std::size_t content_chunk_size)
                    : root_path(std::move(web_root)),
                      index(std::move(index_files)),
                      template_files(std::move(template_files)),
                      template_data_retriever(template_data_retriever),
                      maximum_header_size(max_header_size),
                      content_chunk_size(content_chunk_size)
            {
            }

            const smooth::core::filesystem::Path web_root() const
            {
                return root_path;
            }

            const std::vector<std::string>& indexes() const
            {
                return index;
            }

            const std::set<std::string>& templates() const
            {
                return template_files;
            }

            std::size_t max_header_size() const
            {
                return maximum_header_size;
            }

            std::size_t chunck_size() const
            {
                return content_chunk_size;
            }

            const ITemplateDataRetriever& data_retriever() const
            {
                return template_data_retriever;
            }

        private:
            smooth::core::filesystem::Path root_path{};
            std::vector<std::string> index{};
            std::set<std::string> template_files{};
            const ITemplateDataRetriever& template_data_retriever;
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

        private:
            using HandlerByURL = std::unordered_map<std::string, RequestHandlerSignature>;
            using HandlerByMethod = std::unordered_map<HTTPMethod, HandlerByURL>;

            void handle(HTTPMethod method,
                        IServerResponse& response,
                        const std::string& requested_url,
                        const std::unordered_map<std::string, std::string>& request_headers,
                        const std::unordered_map<std::string, std::string>& request_parameters,
                        const std::vector<uint8_t>& data,
                        MIMEParser& mime,
                        bool fist_part,
                        bool last_part) override;

            smooth::core::filesystem::Path find_index(const smooth::core::filesystem::Path& search_path) const;

            void
            reply_with(IServerResponse& response, std::unique_ptr<responses::IRequestResponseOperation> res);

            smooth::core::Task& task;
            std::shared_ptr<smooth::core::network::ServerSocket<
                    smooth::application::network::http::HTTPServerClient,
                    smooth::application::network::http::HTTPProtocol, IRequestHandler>> server{};

            HandlerByMethod handlers{};
            HTTPServerConfig config{};
            const char* tag = "HTTPServer";
            TemplateProcessor template_processor;
    };


    template<typename ServerSocketType>
    HTTPServer<ServerSocketType>::HTTPServer(smooth::core::Task& task, const HTTPServerConfig& configuration)
            :
            task(task),
            config(std::move(configuration)),
            template_processor(configuration.templates(), config.data_retriever())
    {
    }

    template<typename ServerType>
    void
    HTTPServer<ServerType>::on(HTTPMethod method,
                               const std::string& url,
                               const RequestHandlerSignature& handler)
    {
        handlers[method][url] = handler;
    }

    template<typename ServerType>
    void
    HTTPServer<ServerType>::handle(
            HTTPMethod method,
            IServerResponse& response,
            const std::string& requested_url,
            const std::unordered_map<std::string, std::string>& request_headers,
            const std::unordered_map<std::string, std::string>& request_parameters,
            const std::vector<uint8_t>& data,
            MIMEParser& mime,
            bool fist_part,
            bool last_part)
    {
        using namespace smooth::core::logging;

        // Is there any URLs for the given method?
        auto by_url = handlers.find(method);

        if (by_url != handlers.end())
        {
            // Is there a matching URL?
            auto response_handler = (*by_url).second.find(requested_url);

            if (response_handler == (*by_url).second.end())
            {
                // No handler for this URL, does it match a file path beneath the web root?

                auto found = false;

                Log::info(tag,
                          Format("Request: {1}: '{2}'", Str(utils::http_method_to_string(method)), Str(requested_url)));

                smooth::core::filesystem::Path search{config.web_root()};
                search /= requested_url;

                if (config.web_root().is_parent_of(search) || config.web_root() == search)
                {
                    smooth::core::filesystem::FileInfo info(search);

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
            else
            {
                (*response_handler).second(response,
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
            // No handler for this method type
            reply_with(response, std::make_unique<responses::ErrorResponse>(ResponseCode::Method_Not_Allowed));
        }
    }

    template<typename ServerType>
    smooth::core::filesystem::Path
    HTTPServer<ServerType>::find_index(
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
                                            std::unique_ptr<responses::IRequestResponseOperation> res)
    {
        Log::info(tag, Format("Reply: {1}", Str(response_code_to_text.at(res->get_response_code()))));
        response.reply(std::move(res));
    }
}



