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

namespace smooth::application::io::wiegand
{
    class IWiegandSignal
    {
        public:
            virtual ~IWiegandSignal() = default;

            virtual void wiegand_number(uint8_t num) = 0;

            virtual void wiegand_id(uint32_t id, uint8_t byte_count) = 0;
    };
}
