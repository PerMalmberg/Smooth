// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2019 Per Malmberg (https://github.com/PerMalmberg)
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

#include <nlohmann/json.hpp>

namespace smooth::core::json_util
{
    /// Workaround for json::value() throwing when object is null even though
    /// a default value has been provided.
    /// https://github.com/nlohmann/json/issues/1733
    template<typename T>
    auto default_value(nlohmann::json& json, const std::string& key, T default_value)
    {
        return json.is_object() ? json.value(key, default_value) : default_value;
    }
}