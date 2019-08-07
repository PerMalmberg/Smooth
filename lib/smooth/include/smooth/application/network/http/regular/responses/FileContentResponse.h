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

#include "StringResponse.h"
#include <smooth/core/filesystem/Path.h>
#include <smooth/core/filesystem/Fileinfo.h>

namespace smooth::application::network::http::regular::responses
{
    class FileContentResponse
            : public StringResponse
    {
        public:
            explicit FileContentResponse(smooth::core::filesystem::Path full_path);

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;

            void dump() const override;

        private:
            smooth::core::filesystem::Path path;
            smooth::core::filesystem::FileInfo info;
            std::size_t sent{0};
    };
}