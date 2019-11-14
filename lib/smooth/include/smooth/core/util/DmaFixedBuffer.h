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

// **************************** SPECIAL NOTE ***********************************************
// This class was created with the goal of implementing the requirements for DMA as described
// in ESP-IDF SPI Master API in section "Write and Read Phases".  This class should be useful
// with periperals in the ESP32 chip that support hardware DMA; namely SPI and I2S.

#pragma once

#include <algorithm>
#include <heap/include/esp_heap_caps.h>
#include <smooth/core/logging/log.h>

namespace smooth::core::util
{
    template<typename T, size_t Size, uint32_t MallocCapType>
    class DmaFixedBuffer
    {
        public:
            constexpr size_t size() 
            { 
                return sizeof(T) * Size; 
            }

            DmaFixedBuffer()
            {
                 // Force the user to create DmaFixedBuffer in multiples of 4 bytes
                static_assert(sizeof(T) * Size % 4 == 0,
                              "DmaFixedBuffer must have a length of multiple of 32 bits, i.e. 4 bytes");

                buff = static_cast<T*>(heap_caps_malloc(Size, MallocCapType));

                // Inform user if problems with allocating heap for this DmaFixedBuffer
                if (buff == nullptr)
                {
                    smooth::core::logging::Log::error("DmaFixedBuffer:", "Failed to allocate DmaFixedBuffer");
                }

                // The user has no control were the compiler will place the DmaFixedBuffer at since we can't use
                // alignas to force compiler to place DmaFixedBuffer on a 32 bit aligned address.  But it would
                // be nice to inform user that DmaFixedBuffer address is not 32 bit aligned and that the efficiency
                // of the program will suffer since extra coping will be required.  
                if (reinterpret_cast<int>(&buff) % 4 != 0)
                {
                    smooth::core::logging::Log::warning("DmaFixedBuffer:",
                                                        "Not 32 bit alligned, spi transaction efficiency will be reduced");
                }
            }

            ~DmaFixedBuffer()
            {
                heap_caps_free(buff);
            }

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

        private:
            T* buff;
    };
}
