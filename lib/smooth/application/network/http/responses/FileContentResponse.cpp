#include <utility>
#include <iomanip>
#include <sstream>
#include <smooth/application/network/http/http_utils.h>
#include <smooth/application/network/http/responses/FileContentResponse.h>
#include <smooth/core/filesystem/File.h>
#include <smooth/core/filesystem/Path.h>

using namespace smooth::core::filesystem;

namespace smooth::application::network::http::responses
{
    FileContentResponse::FileContentResponse(smooth::core::filesystem::Path full_path)
            : smooth::application::network::http::responses::Response(ResponseCode::OK),
              path(std::move(full_path)),
              info(path)
    {
    }

    void FileContentResponse::get_headers(std::unordered_map<std::string, std::string>& headers)
    {
        headers["content-length"] = std::to_string(info.size());
        headers["content-type"] = utils::get_content_type(info.path());
        headers["Last-Modified"] = utils::make_http_time(info.last_modified());
        headers["connection"] = "close";
    }

    // Called at least once when sending a response and until ResponseStatus::AllSent is returned
    ResponseStatus FileContentResponse::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
    {
        auto res = ResponseStatus::AllSent;

        if (sent < info.size())
        {
            auto to_send = std::min(info.size() - sent, max_amount);

            auto read_res = smooth::core::filesystem::File::read(path, target, sent, to_send);
            if (!read_res)
            {
                res = ResponseStatus::Error;
            }
            else
            {
                // Anything still left?
                res = sent < info.size() ? ResponseStatus::HasMoreData : ResponseStatus::AllSent;
                sent += to_send;
            }
        }

        return res;
    }
}