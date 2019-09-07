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

#include <chrono>
#include <string>
#include <bitset>
#include <smooth/application/io/wiegand/Wiegand.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;
using namespace smooth::core::timer;
using namespace std::chrono;

static const gpio_num_t D0 = GPIO_NUM_0;
static const gpio_num_t D1 = GPIO_NUM_4;
static const std::chrono::milliseconds timeout(150);

namespace smooth
{
    namespace application
    {
        namespace io
        {
            namespace wiegand
            {
                Wiegand::Wiegand(smooth::core::Task& task, IWiegandSignal& receiver, gpio_num_t d0_pin,
                                 gpio_num_t d1_pin)
                        : receiver(receiver),
                          d0_pin(d0_pin),
                          d1_pin(d1_pin),
                          bit_queue(ISRTaskQueue::create(task, *this)),
                          d0(bit_queue, d0_pin, false, false, GPIO_INTR_NEGEDGE),
                          d1(bit_queue, d1_pin, false, false, GPIO_INTR_NEGEDGE),
                          line_silent(TimerQueue::create("WiegandLineSilent", 5, task, *this)),
                          expire("WiegandLineSilent",
                                 1,
                                 line_silent,
                                 false,
                                 timeout)
                {
                }

                void Wiegand::event(const smooth::core::io::InterruptInputEvent& event)
                {
                    expire->start();
                    ++bit_count;
                    data <<= 1;
                    data.set(0, event.get_io() == d1_pin);
                }

                void Wiegand::event(const smooth::core::timer::TimerExpiredEvent& /*event*/)
                {
                    if (bit_count == 4)
                    {
                        // No integrity data
                        receiver.wiegand_number(static_cast<uint8_t>(data.to_ullong() & 0xF));
                    }
                    else if (bit_count == 8)
                    {
                        // Integrity via high nibble = ~(low nibble), where low nibble is actual data.
                        auto low = data.to_ullong() & 0x0F;
                        auto high = data.to_ullong() & 0xF0;

                        if (low == ~high)
                        {
                            receiver.wiegand_number(static_cast<uint8_t>(low));
                        }
                    }
                    else if (bit_count == 26)
                    {
                        // Integrity data: MSB; even parity for 12 upper data bits, and LSB; odd parity for lower 12
                        // data bits
                        std::bitset<13> upper(data.to_ullong() >> 13);
                        std::bitset<13> lower(data.to_ullong() & 0x1FFF);

                        // Validate even parity on upper part...
                        bool valid = (upper.count() & 1) == 0;

                        // ...and odd parity on lower part
                        valid &= lower.count() & 1;

                        if (valid)
                        {
                            uint32_t id = static_cast<uint32_t>((upper.to_ullong() & 0xFFF) | (lower.to_ullong() >> 1));
                            receiver.wiegand_id(id, 3);
                        }
                    }
                    else if (bit_count == 34)
                    {
                        // Integrity data: MSB; even parity for 16 upper data bits, and LSB; odd parity for lower 16
                        // data bits
                        std::bitset<17> upper(data.to_ullong() >> 17);
                        std::bitset<17> lower(data.to_ullong() & 0x1FFFF);

                        // Validate even parity on upper part...
                        bool valid = (upper.count() & 1) == 0;

                        // ...and odd parity on lower part
                        valid &= lower.count() & 1;

                        if (valid)
                        {
                            uint32_t id = static_cast<uint32_t>((upper.to_ullong() & 0xFFFF)
                                                                | (lower.to_ullong() >> 1));
                            receiver.wiegand_id(id, 4);
                        }
                    }

                    bit_count = 0;
                    data.reset();
                }
            }
        }
    }
}
