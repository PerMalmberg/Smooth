//
// Created by permal on 8/20/17.
//

#pragma once

#include "FixedBufferBase.h"
#include <array>
#include <algorithm>

namespace smooth
{
    namespace core
    {
        namespace util
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
    }
}