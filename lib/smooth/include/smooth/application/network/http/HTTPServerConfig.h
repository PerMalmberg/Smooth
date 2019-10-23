#pragma once

#include <memory>
#include <string>

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

            /// Configuration for HTTPServer (regular and websocket mode)
            /// \arg web_root Full path to the location of the web root where files are served from.
            /// \arg index_files List of file names which are treated as index files, e.g. "index.html"
            /// \arg template_files List of file extensions for files that are to be treated as templates.
            /// \arg template_data_retriever The instance that provides data to fill templates with.
            /// \arg max_header_size Maximum size of HTTP headers to allow. If a header larger than the specified value
            /// is received the connection is closed.
            /// \arg content_chunk_size The chunk size, in bytes, incoming data is split into. This split occurs to
            /// prevent large HTTP POST/GET requests from consuming memory.
            /// \arg max_enqueued_responses Each response takes up a little bit of memory (more so if responding with
            /// data). To prevent an out-of-memory situation when there is a steady stream of incoming data, and the
            /// device can't send out the responses fast enough, this threshold protects the device by closing the
            /// connection if it is reached.
            HTTPServerConfig(smooth::core::filesystem::Path web_root,
                             std::vector<std::string> index_files,
                             std::set<std::string> template_files,
                             std::shared_ptr<ITemplateDataRetriever> template_data_retriever,
                             std::size_t max_header_size,
                             std::size_t content_chunk_size,
                             std::size_t max_enqueued_responses)
                    : root_path(std::move(web_root)),
                      index(std::move(index_files)),
                      template_files(std::move(template_files)),
                      template_data_retriever(std::move(template_data_retriever)),
                      maximum_header_size(max_header_size),
                      content_chunk_size(content_chunk_size),
                      max_enqueued_responses(max_enqueued_responses)
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

            [[nodiscard]] std::size_t chunk_size() const
            {
                return content_chunk_size;
            }

            [[nodiscard]] std::shared_ptr<ITemplateDataRetriever> data_retriever() const
            {
                return template_data_retriever;
            }

            [[nodiscard]] std::size_t max_responses() const
            {
                return max_enqueued_responses;
            }

        private:
            smooth::core::filesystem::Path root_path{};
            std::vector<std::string> index{};
            std::set<std::string> template_files{};
            std::shared_ptr<ITemplateDataRetriever> template_data_retriever{};
            std::size_t maximum_header_size{};
            std::size_t content_chunk_size{};
            std::size_t max_enqueued_responses{};
    };
}
