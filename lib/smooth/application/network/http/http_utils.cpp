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

#include <smooth/application/network/http/http_utils.h>

#include <array>
#include <sstream>
#include <iomanip>
#include <mutex>

using namespace std::chrono;
using namespace smooth::application::network::http::regular;

namespace smooth::application::network::http::utils
{
    static const std::array<const char*, 7> day{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const std::array<const char*, 12> month{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
                                                   "Nov", "Dec"};

    std::string make_http_time(const std::chrono::system_clock::time_point& t)
    {
        auto tt = std::chrono::system_clock::to_time_t(t);
        return make_http_time(tt);
    }

    std::string make_http_time(const time_t& t)
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

    std::chrono::system_clock::time_point parse_http_time(const std::string& t)
    {
        tm time{};

        // Default locale is "C"
        std::istringstream ss(t);

        system_clock::time_point res{};

        ss >> std::get_time(&time, "%a, %d %b %Y %H:%M:%S GMT");
        if(ss.fail())
        {
            res = system_clock::time_point::min();
        }
        else
        {
            time.tm_isdst = 0;
            time_t time_to_use = timegm(time);
            res = system_clock::from_time_t(time_to_use);
        }

        return res;
    }

    std::string get_content_type(const smooth::core::filesystem::Path& path)
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

    time_t timegm(tm& tm)
    {
        static std::mutex timemg_lock{};
        std::lock_guard<std::mutex> lock(timemg_lock);

        time_t ret{};
        char* tz = nullptr;

        tz = getenv("TZ");
        setenv("TZ", "", 1);
        tzset();
        ret = std::mktime(&tm);

        if (tz)
        {
            setenv("TZ", tz, 1);
        }
        else
        {
            unsetenv("TZ");
        }

        tzset();

        return ret;
    }

    std::string http_method_to_string(const HTTPMethod m)
    {
        if(m == HTTPMethod::DELETE)
        {
            return "DELETE";
        }
        else if(m == HTTPMethod::GET)
        {
            return "GET";
        }
        else if(m == HTTPMethod::HEAD)
        {
            return "HEAD";
        }
        else if(m == HTTPMethod::PUT)
        {
            return "PUT";
        }
        else if(m == HTTPMethod::POST)
        {
            return "POST";
        }
        return "";
    }
}