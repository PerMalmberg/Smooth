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

#ifdef ESP_PLATFORM
#include "esp_attr.h"
#define SMOOTH_MEM_ATTR IRAM_ATTR
#else
#define SMOOTH_MEM_ATTR
#endif

#include <type_traits>

namespace smooth::core::ipc
{
    /// Interface for ISR-safe task queue, see ISRTaskEventQueue fore more details.
    /// \tparam DataType The type of data to send from the ISR.
    template<typename DataType>
    class IISRTaskEventQueue
    {
        static_assert(std::is_default_constructible<DataType>::value, "DataType must be default-constructible");
        static_assert(std::is_trivial<DataType>::value, "DataType must be a trivial type");
        public:
            virtual ~IISRTaskEventQueue() = default;

            /// When called from an ISR, signals the queue to create an event.
            /// \param data The data to attach to the signal.
            SMOOTH_MEM_ATTR virtual void signal(const DataType& data) = 0;
    };
}
