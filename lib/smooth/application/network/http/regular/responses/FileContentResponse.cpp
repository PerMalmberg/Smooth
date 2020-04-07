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

#include <utility>
#include <iomanip>
#include <sstream>
#include "smooth/application/network/http/http_utils.h"
#include "smooth/core/filesystem/File.h"
#include "smooth/core/filesystem/Path.h"
#include "smooth/application/network/http/regular/responses/FileContentResponse.h"
#include "smooth/application/network/http/regular/HTTPHeaderDef.h"

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
        Log::debug("FileContentResponse", "Code: {}; Status: {}/{} bytes, Path: {}", code, sent, info.size(), path);
    }
}
