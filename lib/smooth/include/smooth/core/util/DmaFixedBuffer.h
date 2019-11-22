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

#include <algorithm>
#include <esp_heap_caps.h>
#include "smooth/core/logging/log.h"

namespace smooth::core::util
{
    template<typename T, size_t Size>
    class DmaFixedBuffer
    {
        public:
            constexpr size_t size()
            {
                return sizeof(T) * Size;
            }

            // Constructor
            DmaFixedBuffer(const uint32_t malloc_cap_type)
            {
                buff = static_cast<T*>(heap_caps_malloc(Size, malloc_cap_type));
            }

            // Destructor
            virtual ~DmaFixedBuffer()
            {
                heap_caps_free(buff);
            }

            /// \return Returns pointer to first element in buffer
            T* data()
            {
                return buff;
            }

            const T& operator[](size_t index) const
            {
                // Prevent going outside buffer
                return buff[std::max(0u, std::min(size() - 1, index))];
            }

            T& operator[](size_t index)
            {
                // Prevent going outside buffer
                return buff[std::max(static_cast<size_t>(0), std::min(size() - 1, index))];
            }

        protected:
            T* buff;
    };
}
