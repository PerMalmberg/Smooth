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

#include <smooth/core/logging/log.h>
#include "esp_log.h"

namespace smooth::core::logging
{
    void Log::error(const std::string& tag, const std::string& msg)
    {
        ESP_LOGE(tag.c_str(), "%s", msg.c_str());
    }

    void Log::warning(const std::string& tag, const std::string& msg)
    {
        ESP_LOGW(tag.c_str(), "%s", msg.c_str());
    }

    void Log::info(const std::string& tag, const const std::string& msg)
    {
        ESP_LOGI(tag.c_str(), "%s", msg.c_str());
    }

    void Log::debug(const std::string& tag, const std::string& msg)
    {
        ESP_LOGD(tag.c_str(), "%s", msg.c_str());
    }

    void Log::verbose(const std::string& tag, const std::string& msg)
    {
        ESP_LOGV(tag.c_str(), "%s", msg.c_str());
    }
}
