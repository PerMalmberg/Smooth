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

#pragma once

#include <memory>
#include <set>
#include <regex>
#include <string>
#include "smooth/core/filesystem/Path.h"
#include "smooth/application/network/http/regular/responses/StringResponse.h"
#include "ITemplateDataRetriever.h"

namespace smooth::application::network::http::regular
{
    class TemplateProcessor
    {
        public:
            explicit TemplateProcessor(std::set<std::string> template_files,
                                       std::shared_ptr<ITemplateDataRetriever> data_retriever);

            std::unique_ptr<smooth::application::network::http::IResponseOperation>
            process_template(const smooth::core::filesystem::Path& path);

#ifndef EXPOSE_PRIVATE_PARTS_FOR_TEST
        private:
#endif

            void process_template(std::string& template_data) const;

            std::set<std::string> template_files;
            std::shared_ptr<ITemplateDataRetriever> data_retriever;
            const std::regex token{ R"!(\{\{[\d\_\-a-zA-Z]+\}\})!", std::regex::ECMAScript };
    };
}
