//
// Created by permal on 7/2/17.
//

#pragma once

#include <algorithm>
#include <esp_log.h>

namespace smooth
{
    namespace network
    {
        class ISendBuffer
        {
            public:
                virtual int size() = 0;
                virtual const char* data() = 0;
                virtual void take(int count);
        };

        template<int buff_size>
        class SendBuffer
                : public ISendBuffer
        {
            public:
                SendBuffer(const char* data, int length)
                {
                    int i = 0;
                    int len = std::min(buff_size, length);
                    for (; i < len; ++i)
                    {
                        buff[i] = data[i];
                    }

                    taken = buff_size - len;
                    ESP_LOGV("SendBuffer", "taken: %d, size %d, len: %d", taken, buff_size, len)
                }

                const char* data() override
                {
                    return buff + taken;
                }

                int size() override
                {
                    return std::max(0, buff_size - taken);
                }

                void take(int count) override
                {
                    taken += count;
                    ESP_LOGV("Sendbuffer", "Took %d, size is now: %d", count, size());
                }

            private:
                char buff[buff_size];
                int taken = 0;
        };
    }
}