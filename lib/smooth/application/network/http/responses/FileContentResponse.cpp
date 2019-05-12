#include <utility>


#include <smooth/application/network/http/responses/FileContentResponse.h>
#include <smooth/core/filesystem/File.h>

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
                    FileContentResponse::FileContentResponse(std::filesystem::path full_path)
                            : smooth::application::network::http::responses::Response(ResponseCode::OK),
                              path(std::move(full_path)),
                              file_size(static_cast<int64_t>(std::filesystem::file_size(path)))
                    {

                    }

                    void FileContentResponse::get_headers(std::unordered_map<std::string, std::string>& headers)
                    {
                        headers["content-length"] = std::to_string(file_size);
                        headers["content-type"] = get_content_type(path);
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

                    std::string FileContentResponse::get_content_type(const std::filesystem::path& path)
                    {
                        (void) path;
                        return "application/octet-stream";
                    }
                }
            }
        }
    }
}