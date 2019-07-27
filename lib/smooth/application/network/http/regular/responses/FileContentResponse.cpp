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

#include <utility>
#include <iomanip>
#include <sstream>
#include <smooth/application/network/http/http_utils.h>
#include <smooth/core/filesystem/File.h>
#include <smooth/core/filesystem/Path.h>
#include <smooth/application/network/http/regular/responses/FileContentResponse.h>
#include <smooth/application/network/http/regular/HTTPHeaderDef.h>

using namespace smooth::core::filesystem;

namespace smooth::application::network::http::regular::responses
{
    FileContentResponse::FileContentResponse(smooth::core::filesystem::Path full_path)
            : StringResponse(ResponseCode::OK),
              path(std::move(full_path)),
              info(path)
    {
        headers[CONTENT_LENGTH] = std::to_string(info.size());
        headers[CONTENT_TYPE] = utils::get_content_type(info.path());
        headers[LAST_MODIFIED] = utils::make_http_time(info.last_modified());
    }

    // Called at least once when sending a response and until ResponseStatus::NoData is returned
    ResponseStatus FileContentResponse::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
    {
        auto res = ResponseStatus::NoData;

        if (sent < info.size())
        {
            auto to_send = std::min(info.size() - sent, max_amount);

            auto read_res = smooth::core::filesystem::File::read(path, target, sent, to_send);
            if (read_res)
            {
                sent += to_send;
                res = sent < info.size() ? ResponseStatus::HasMoreData : ResponseStatus::LastData;
            }
            else
            {
                res = ResponseStatus::Error;
            }
        }

        return res;
    }

    void FileContentResponse::dump() const
    {
        Log::debug("FileContentResponse", Format("Code: {1}; Status: {2}/{3} bytes, Path: {4}", Int32{static_cast<int>(code)}, UInt64{sent}, UInt64{info.size()}, Str{static_cast<const std::string&>(path)}));
    }
}