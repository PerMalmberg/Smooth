//
// Created by permal on 7/5/17.
//

#pragma once

#include <smooth/util/CircularBuffer.h>

namespace smooth
{
    namespace network
    {
        class TransmitBufferEmpty
        {
            public:
                TransmitBufferEmpty() = default;

                TransmitBufferEmpty(smooth::util::ICircularBuffer<char>* tx) : tx(tx)
                {
                }

                smooth::util::ICircularBuffer<char>* get_tx() const
                {
                    return tx;
                }

            private:
                smooth::util::ICircularBuffer<char>* tx;
        };
    }
}