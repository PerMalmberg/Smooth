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

#include <smooth/core/util/DmaFixedBuffer.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

namespace smooth::core::io::spi
{
    template<typename T, size_t Size>
    class SpiDmaFixedBuffer : public smooth::core::util::DmaFixedBuffer<T, Size>
    {
        public:
            static constexpr const char* TAG = "SpiDmaFixedBuffer";

            // Constructor derived class of DmaFixedBuffer
            SpiDmaFixedBuffer() : smooth::core::util::DmaFixedBuffer<T, Size>(MALLOC_CAP_DMA)
            {
                // The SPI Master requires the DMA data to be length of multiples of 32 bits to avoid heap corruption.
                static_assert(Size % 4 == 0, "spi dma buffer must have a length of multiple of 32 bits, i.e. 4 bytes");
            }

            // Destructor
            ~SpiDmaFixedBuffer() = default;

            /// Checks to see if buffer has been allocated
            /// \return Returns true if buffer is allocated; false if buffer is not allocted (no space on heap)
            bool is_buffer_allocated()
            {
                bool res = this->data() != nullptr;

                if (!res)
                {
                    Log::error(TAG, "spi dma buffer has NOT been allocated");
                }

                return res;
            }

            /// Checks if buffer address is aligned on a 32 bit boundary
            void is_buffer_address_32_bit_aligned()
            {
                if (reinterpret_cast<uint32_t>(&this->buff) % 4 != 0)
                {
                    Log::warning(TAG,
                "spi dma buffer is not 32 bit alligned, spi transaction efficiency will be reduced");
                }
            }

            /// Provide user a method to check if buffer is valid
            /// \return Retuns true if buffer is valid false if something went wrong
            bool is_valid()
            {
                is_buffer_address_32_bit_aligned();
                return is_buffer_allocated();
            }
    };
}
