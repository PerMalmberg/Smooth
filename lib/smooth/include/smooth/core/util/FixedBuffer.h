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

#include "FixedBufferBase.h"
#include <array>
#include <algorithm>

namespace smooth::core::util
{
    /// \brief FixedBuffer<T> adds range checking on top if std::array<T>
    template<typename T, size_t Size>
    class FixedBuffer
        : public FixedBufferBase<T>
    {
        public:
            [[nodiscard]] size_t size() const override
            {
                return buff.size();
            }

            typename std::array<T, Size>::iterator begin()
            {
                return buff.begin();
            }

            typename std::array<T, Size>::iterator end()
            {
                return buff.end();
            }

            T* data() override
            {
                return &buff[0];
            }

            const T& operator[](size_t ix) const
            {
                // Prevent going outside buffer
                return buff[std::max(0u, std::min(size() - 1, ix))];
            }

            T& operator[](size_t ix)
            {
                // Prevent going outside buffer
                return buff[std::max(static_cast<size_t>(0), std::min(size() - 1, ix))];
            }

        private:
            std::array<T, Size> buff;
    };
}
