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
        class ITransferBuffer
        {
            public:
                virtual int size() = 0;
                virtual const char* data() = 0;
                virtual void take(int count);
        };

        template<int buff_size>
        class TransferBuffer
                : public ITransferBuffer
        {
            public:
                TransferBuffer() = default;

                void transfer(const char* data, int length)
                {
                    int len = std::min(buff_size, length);
                    memcpy(buff, data, len);
                    left_in_buffer = len;
                    ESP_LOGV("SendBuffer", "left_in_buffer: %d, size %d, len: %d", left_in_buffer, buff_size, len)
                }

                const char* data() override
                {
                    return buff +  buff_size - left_in_buffer;
                }

                int size() override
                {
                    return left_in_buffer;
                }

                void take(int count) override
                {
                    left_in_buffer = std::max(0, left_in_buffer - count);
                    ESP_LOGV("Sendbuffer", "Took %d, size is now: %d", count, size());
                }

            private:
                char buff[buff_size];
                int left_in_buffer = 0;
        };
    }
}