//
// Created by permal on 12/4/17.
//

#include "Wiegand.h"
#include <chrono>
#include <string>
#include <bitset>
#include <smooth/core/logging/log.h>


using namespace std::chrono;
using namespace smooth::core::logging;
using namespace smooth::core::timer;

static const gpio_num_t D0 = GPIO_NUM_0;
static const gpio_num_t D1 = GPIO_NUM_4;
static const std::chrono::milliseconds timeout(150);

Wiegand::Wiegand(smooth::core::Task& task, IWiegandSignal& receiver)
        : receiver(receiver),
          bit_queue(task, *this),
          d0(bit_queue, D0, false, false, GPIO_INTR_NEGEDGE),
          d1(bit_queue, D1, false, false, GPIO_INTR_NEGEDGE),
          line_silent("WiegandLineSilent", 5, task, *this),
          expire(Timer::create("WiegandLineSilent",
                               1,
                               line_silent,
                               false,
                               timeout))
{
}

void Wiegand::event(const smooth::core::io::InterruptInputEvent& event)
{
    expire->start();
    ++bit_count;
    data <<= 1;
    data.set(0, event.get_io() == D1);
}

void Wiegand::event(const smooth::core::timer::TimerExpiredEvent& event)
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
        // Integrity data: MSB; even parity for 12 upper data bits, and LSB; odd parity for lower 12 data bits
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
        // Integrity data: MSB; even parity for 16 upper data bits, and LSB; odd parity for lower 16 data bits
        std::bitset<17> upper(data.to_ullong() >> 17);
        std::bitset<17> lower(data.to_ullong() & 0x1FFFF);

        // Validate even parity on upper part...
        bool valid = (upper.count() & 1) == 0;
        // ...and odd parity on lower part
        valid &= lower.count() & 1;

        if (valid)
        {
            uint32_t id = static_cast<uint32_t>((upper.to_ullong() & 0xFFFF) | (lower.to_ullong() >> 1));
            receiver.wiegand_id(id, 4);
        }
    }

    bit_count = 0;
    data.reset();
}