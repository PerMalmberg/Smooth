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

#include "smooth/application/network/http/regular/TemplateProcessor.h"
#include <regex>
#include "smooth/application/network/http/regular/responses/StringResponse.h"
#include "smooth/application/network/http/regular/responses/ErrorResponse.h"
#include "smooth/application/network/http/regular/ResponseCodes.h"
#include "smooth/core/filesystem/File.h"
#include "smooth/core/util/string_util.h"

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
