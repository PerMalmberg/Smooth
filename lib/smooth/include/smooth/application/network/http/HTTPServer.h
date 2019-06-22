#include <utility>

#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include "HTTPProtocol.h"
#include "HTTPMethod.h"
#include <smooth/core/Task.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/filesystem/File.h>
#include <smooth/core/filesystem/Fileinfo.h>
#include <smooth/core/filesystem/Path.h>
#include <smooth/core/network/InetAddress.h>
#include <smooth/core/network/ServerSocket.h>
#include <smooth/core/network/SecureServerSocket.h>
#include <smooth/application/network/http/http_utils.h>
#include <smooth/application/network/http/HTTPProtocol.h>
#include <smooth/application/network/http/HTTPServerClient.h>
#include <smooth/application/network/http/responses/EmptyResponse.h>
#include <smooth/application/network/http/responses/FileContentResponse.h>
#include "ResponseSignature.h"

namespace smooth::application::network::http
{
    // https://upload.wikimedia.org/wikipedia/commons/8/88/Http-headers-status.png

    class HTTPServerConfig
    {
        public:
            HTTPServerConfig() = default;

            HTTPServerConfig(const HTTPServerConfig&) = default;

            HTTPServerConfig(HTTPServerConfig&&) = default;

            HTTPServerConfig& operator=(const HTTPServerConfig&) = default;

            HTTPServerConfig& operator=(HTTPServerConfig&&) = default;

            HTTPServerConfig(smooth::core::filesystem::Path web_root,
                             std::vector<std::string> index_files,
                             int max_header_size,
                             int content_chunk_size)
                    : root_path(std::move(web_root)),
                      index(std::move(index_files)),
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

            int max_header_size() const
            {
                return maximum_header_size;
            }

            int chunck_size() const
            {
                return content_chunk_size;
            }

        private:
            smooth::core::filesystem::Path root_path{};
            std::vector<std::string> index{};
            int maximum_header_size{};
            int content_chunk_size{};
    };

    template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
    class HTTPServer
            : private IRequestHandler<MaxHeaderSize, ContentChuckSize>
    {
        public:
            HTTPServer(smooth::core::Task& task, HTTPServerConfig configuration);

            void start(int max_client_count, int backlog, std::shared_ptr<smooth::core::network::InetAddress> bind_to)
            {
                server = ServerType::create(task, max_client_count, backlog);
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

            smooth::core::filesystem::Path find_index(const smooth::core::filesystem::Path& search_path) const;

            void
            reply_with(IResponseQueue& response, std::unique_ptr<responses::IRequestResponseOperation> res);

            smooth::core::Task& task;
            std::shared_ptr<smooth::core::network::ServerSocket<
                    smooth::application::network::http::HTTPServerClient<MaxHeaderSize, ContentChuckSize>,
                    smooth::application::network::http::HTTPProtocol<MaxHeaderSize, ContentChuckSize>>> server{};

            HandlerByMethod handlers{};
            HTTPServerConfig config{};
            const char* tag = "HTTPServer";
    };


    template<typename ServerSocketType, int MaxHeaderSize, int ContentChuckSize>
    HTTPServer<ServerSocketType, MaxHeaderSize, ContentChuckSize>::HTTPServer(smooth::core::Task& task,
                                                                              HTTPServerConfig configuration)
            :
            task(task),
            config(std::move(configuration))
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
                        // Serve the requested file
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
                                       std::make_unique<responses::EmptyResponse>(ResponseCode::Not_Modified));
                        }
                        else
                        {
                            reply_with(response, std::make_unique<responses::FileContentResponse>(search));
                        }

                        found = true;
                    }
                    else if (info.is_directory())
                    {
                        auto index_path = find_index(search);
                        if (!index_path.empty())
                        {
                            reply_with(response, std::make_unique<responses::FileContentResponse>(index_path));
                            found = true;
                        }
                    }
                }

                if (!found)
                {
                    reply_with(response, std::make_unique<responses::EmptyResponse>(ResponseCode::Not_Found));
                }
            }
            else
            {
                Log::info(tag, Format("Matching route: {1}: '{2}'", Str(utils::http_method_to_string(method)),
                                      Str(requested_url)));

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
            reply_with(response, std::make_unique<responses::EmptyResponse>(ResponseCode::Method_Not_Allowed));
        }
    }

    template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
    smooth::core::filesystem::Path
    HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::find_index(
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

    template<typename ServerType, int MaxHeaderSize, int ContentChuckSize>
    void HTTPServer<ServerType, MaxHeaderSize, ContentChuckSize>::reply_with(IResponseQueue& response,
                                                                             std::unique_ptr<responses::IRequestResponseOperation> res)
    {
        Log::info(tag, Format("Reply: {1}", Str(response_code_to_text.at(res->get_response_code()))));
        response.enqueue(std::move(res));
    }
}



