//
// Created by permal on 7/5/17.
//

#pragma once

#include <smooth/util/CircularBuffer.h>

namespace smooth
{
    namespace network
    {
        class DataAvailable
        {
            public:
                DataAvailable() = default;

                DataAvailable(smooth::util::ICircularBuffer<char>* rx) : rx(rx)
                {
                }

                smooth::util::ICircularBuffer<char>* get_rx() const
                {
                    return rx;
                }

            private:
                smooth::util::ICircularBuffer<char>* rx;
        };
    }
}