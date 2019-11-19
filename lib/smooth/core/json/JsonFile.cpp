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

#include "smooth/core/json/JsonFile.h"
#include <memory>
#include <vector>

namespace smooth::core::json
{
    JsonFile::JsonFile(const char* full_path)
            : f(full_path)
    {
        load();
    }

    JsonFile::JsonFile(const smooth::core::filesystem::Path& full_path)
            : f(full_path)
    {
        load();
    }

    void JsonFile::load()
    {
        if (exists())
        {
            std::vector<uint8_t> data;
            f.read(data);

            if (!data.empty())
            {
                // Append terminating zero.
                data.push_back(0);
                v = nlohmann::json::parse(data.data(), nullptr, false);
            }
        }
    }

    bool JsonFile::save() const
    {
        return f.write(v.dump(4, ' '));
    }
}
