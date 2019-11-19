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

#include "smooth/application/io/wiegand/Wiegand.h"

using namespace smooth::core::logging;
using namespace smooth::core::timer;
using namespace std::chrono;

static const gpio_num_t D0 = GPIO_NUM_0;
static const gpio_num_t D1 = GPIO_NUM_4;
static const int LINE_SILENT = 1;
static constexpr std::chrono::milliseconds timeout(25);

namespace smooth::application::io::wiegand
{
    Wiegand::Wiegand(smooth::core::Task& task, IWiegandSignal& receiver, gpio_num_t d0_pin, gpio_num_t d1_pin)
            : receiver(receiver),
              d0_pin(d0_pin),
              d1_pin(d1_pin),
              key_queue(KeyQueue::create(task, *this)),
              number_queue(NumberQueue::create(task, *this)),
              tick_queue(TimerQueue::create(1, task, *this)),
              tick(0, tick_queue, true, timeout),
              d0(&Wiegand::isr_d0, this, d0_pin, false, false, GPIO_INTR_NEGEDGE),
              d1(&Wiegand::isr_d1, this, d1_pin, false, false, GPIO_INTR_NEGEDGE)
    {
        tick->start();
    }

    void Wiegand::clear_bits()
    {
        bit_count = 0;
        data.reset();
    }

    void Wiegand::parse()
    {
        if (bit_count > 0 && esp_timer_get_time() - last_data > timeout.count() * 1000)
        {
            gpio_intr_disable(d0_pin);
            gpio_intr_disable(d1_pin);

            if (bit_count == 4)
            {
                // No integrity data
                key_queue->signal(static_cast<uint8_t>(data.to_ullong() & 0xFu));
            }
            else if (bit_count == 8)
            {
                // Integrity via high nibble = ~(low nibble), where low nibble is actual data.
                auto low = data.to_ullong() & 0x0Fu;
                auto high = data.to_ullong() & 0xF0u;

                if (low == ~high)
                {
                    key_queue->signal(static_cast<uint8_t>(low));
                }
            }
            else if (bit_count == 26)
            {
                // Integrity data: MSB; even parity for 12 upper data bits, and LSB; odd parity for lower 12
                // data bits
                std::bitset<13> upper(data.to_ullong() >> 13u);
                std::bitset<13> lower(data.to_ullong() & 0x1FFFu);

                // Validate even parity on upper part...
                bool valid = (upper.count() & 1u) == 0;

                // ...and odd parity on lower part
                valid &= lower.count() & 1u;

                if (valid)
                {
                    auto id = static_cast<uint32_t>((upper.to_ullong() & 0xFFFu) | (lower.to_ullong() >> 1u));
                    number_queue->signal(id);
                }
            }
            else if (bit_count == 34)
            {
                // Integrity data: MSB; even parity for 16 upper data bits, and LSB; odd parity for lower 16
                // data bits
                std::bitset<17> upper(data.to_ullong() >> 17u);
                std::bitset<17> lower(data.to_ullong() & 0x1FFFFu);

                // Validate even parity on upper part...
                bool valid = (upper.count() & 1u) == 0;

                // ...and odd parity on lower part
                valid &= lower.count() & 1u;

                if (valid)
                {
                    auto id = static_cast<uint32_t>((upper.to_ullong() & 0xFFFFu)
                                                    | (lower.to_ullong() >> 1u));
                    number_queue->signal(id);
                }
            }

            clear_bits();

            gpio_intr_enable(d0_pin);
            gpio_intr_enable(d1_pin);
        }
    }

    void Wiegand::isr_d0(void* context)
    {
        // Negative pulse on D0 means a 0
        auto w = reinterpret_cast<Wiegand*>(context);
        w->zero();
    }

    void Wiegand::isr_d1(void* context)
    {
        // Negative pulse on D1 means a 1
        auto w = reinterpret_cast<Wiegand*>(context);
        w->one();
    }

    void Wiegand::zero()
    {
        ++bit_count;
        data <<= 1;
        last_data = esp_timer_get_time();
    }

    void Wiegand::one()
    {
        ++bit_count;
        data <<= 1;
        data.set(0, true);
        last_data = esp_timer_get_time();
    }

    void Wiegand::event(const uint8_t& number)
    {
        receiver.wiegand_number(number);
    }

    void Wiegand::event(const uint32_t& id)
    {
        receiver.wiegand_id(id);
    }

    void Wiegand::event(const smooth::core::timer::TimerExpiredEvent& /*event*/)
    {
        parse();
    }
}
