// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
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

#include "FixedBufferBase.h"
#include <array>
#include <algorithm>

namespace smooth::core::util
{
    template<typename T, size_t Size>
    class FixedBuffer
            : public FixedBufferBase<T>
    {
        public:
            size_t size() const override
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
                return buff[std::max(0u, std::min(size() - 1, ix))];
            }

        private:
            std::array<T, Size> buff;
    };
}