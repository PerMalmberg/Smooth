#include <utility>


#include <smooth/application/network/http/responses/FileContentResponse.h>
#include <smooth/core/filesystem/File.h>
#include <smooth/core/filesystem/Path.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                namespace responses
                {
                    FileContentResponse::FileContentResponse(smooth::core::filesystem::Path full_path)
                            : smooth::application::network::http::responses::Response(ResponseCode::OK),
                              path(std::move(full_path)),
                              file_size(static_cast<int64_t>(smooth::core::filesystem::File::file_size(path)))
                    {

                    }

                    void FileContentResponse::get_headers(std::unordered_map<std::string, std::string>& headers)
                    {
                        headers["content-length"] = std::to_string(file_size);
                        headers["content-type"] = get_content_type();
                        headers["connection"] = "close";
                    }

                    // Called at least once when sending a response and until ResponseStatus::AllSent is returned
                    ResponseStatus FileContentResponse::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
                    {
                        auto res = ResponseStatus::AllSent;

                        if (sent < file_size)
                        {
                            auto to_send = static_cast<int64_t>(std::min(file_size - sent,
                                                                         static_cast<int64_t>(max_amount)));

                            auto read_res = smooth::core::filesystem::File::read(path, target, sent, to_send);
                            if (!read_res)
                            {
                                res = ResponseStatus::Error;
                            }
                            else
                            {
                                // Anything still left?
                                res = sent < file_size ? ResponseStatus::HasMoreData : ResponseStatus::AllSent;
                                sent += to_send;
                            }
                        }

                        return res;
                    }

                    std::string FileContentResponse::get_content_type()
                    {
                        // https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Complete_list_of_MIME_types
                        const auto& ext = path.extension();
                        if (ext == ".jpeg")
                        {
                            return "image/jpeg";
                        }
                        else if (ext == ".html")
                        {
                            return "text/html";
                        }
                        return "application/octet-stream";
                    }
                }
            }
        }
    }
}