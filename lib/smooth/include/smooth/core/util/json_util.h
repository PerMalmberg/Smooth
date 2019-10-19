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

#include <nlohmann/json.hpp>

namespace smooth::core::json_util
{
    /// \brief Workaround for json::value() throwing when object is null even though
    /// a default value has been provided.
    /// https://github.com/nlohmann/json/issues/1733
    template<typename T>
    auto default_value(nlohmann::json& json, const std::string& key, T default_value)
    {
        return json.is_object() ? json.value(key, default_value) : default_value;
    }
}
