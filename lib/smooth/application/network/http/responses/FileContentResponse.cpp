#include <utility>
#include <iomanip>
#include <sstream>
#include <smooth/application/network/http/responses/FileContentResponse.h>
#include <smooth/core/filesystem/File.h>
#include <smooth/core/filesystem/Path.h>

using namespace smooth::core::filesystem;

namespace smooth::application::network::http::responses
{
    static const std::array<const char*,7> day{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const std::array<const char*,12> month{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    FileContentResponse::FileContentResponse(smooth::core::filesystem::Path full_path)
            : smooth::application::network::http::responses::Response(ResponseCode::OK),
              path(std::move(full_path)),
              info(path)
    {
    }

    void FileContentResponse::get_headers(std::unordered_map<std::string, std::string>& headers)
    {
        headers["content-length"] = std::to_string(info.size());
        headers["content-type"] = get_content_type();
        headers["Last-Modified"] = format_last_modified(info.last_modified());
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

    std::string FileContentResponse::format_last_modified(const time_t& t) const
    {
        tm time{};
        gmtime_r(&t, &time);
        std::stringstream ss;

        // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Last-Modified
        // GMT == UTC and the time read from disc is in UTC so no need to convert between timezones.
        // <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
        ss << day[static_cast<decltype(day)::size_type>(time.tm_wday)];
        ss << ", " << std::setfill('0') << std::setw(2) << time.tm_mday;
        ss << " " << month[static_cast<decltype(month)::size_type>(time.tm_mon)];
        ss << " " << time.tm_year + 1900;
        ss << " " << std::setfill('0') << std::setw(2) << time.tm_hour;
        ss << ":" << std::setfill('0') << std::setw(2) << time.tm_min;
        ss << ":" << std::setfill('0') << std::setw(2) << time.tm_sec;
        ss << " GMT";

        return ss.str();
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