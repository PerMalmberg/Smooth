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

#include <smooth/application/network/http/regular/TemplateProcessor.h>
#include <regex>
#include <smooth/application/network/http/regular/responses/StringResponse.h>
#include <smooth/application/network/http/regular/responses/ErrorResponse.h>
#include <smooth/application/network/http/regular/ResponseCodes.h>
#include <smooth/core/filesystem/File.h>
#include <smooth/core/util/string_util.h>

using namespace smooth::core::filesystem;
using namespace smooth::core::string_util;

namespace smooth::application::network::http::regular
{
    TemplateProcessor::TemplateProcessor(std::set<std::string> template_files,
                                         std::shared_ptr<ITemplateDataRetriever> data_retriever)
            : template_files(std::move(template_files)),
              data_retriever(std::move(data_retriever))
    {
    }

    std::unique_ptr<IResponseOperation> TemplateProcessor::process_template(const smooth::core::filesystem::Path& path)
    {
        std::unique_ptr<IResponseOperation> res{};

        const auto& ext = path.extension();
        bool is_template_file = template_files.find(ext) != template_files.end();

        if (is_template_file)
        {
            std::string data;
            File src{ path };

            if (!src.read(data) || data.empty())
            {
                res = std::make_unique<responses::ErrorResponse>(ResponseCode::Internal_Server_Error);
            }
            else
            {
                process_template(data);
                res = std::make_unique<responses::StringResponse>(ResponseCode::OK, data, false);
            }
        }

        return res;
    }

    void TemplateProcessor::process_template(std::string& template_data) const
    {
        // Find keys in the form "{{alpha_num}}, then do a lookup on the alpha_num part
        // and replace the entire found token. Continue search after the found token until end of data.

        if (data_retriever)
        {
            try
            {
                // Find all tokens
                std::smatch match{};

                while (std::regex_search(template_data, match, token))
                {
                    std::stringstream ss;
                    const auto& found_token = match[0].str();

                    // Replaces any matched token, even if no corresponding data exists, in which case empty string is
                    // used.
                    replace_all(template_data, found_token, data_retriever->get(found_token));
                }
            }
            catch (std::regex_error& ex)
            {
                Log::error("TemplateProcessor", ex.what());
            }
        }
    }
}
