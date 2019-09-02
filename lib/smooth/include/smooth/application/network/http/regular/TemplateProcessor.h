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

#pragma once

#include <memory>
#include <set>
#include <regex>
#include <string>
#include <smooth/core/filesystem/Path.h>
#include <smooth/application/network/http/regular/responses/StringResponse.h>
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
