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

#include <smooth/application/rgb_led/RGBLed.h>
#include <smooth/core/util/ByteSet.h>
#include <cstring>
#include <driver/periph_ctrl.h>

namespace smooth::application::rgb_led
{
    RGBLed::RGBLed(rmt_channel_t channel, gpio_num_t io_pin, uint16_t led_count, const RGBLedTiming& timing)
            : led_data(), channel(channel), led_count(led_count)
    {
        periph_module_enable(PERIPH_RMT_MODULE);
        ESP_ERROR_CHECK(rmt_driver_install(channel, 0, 0));

        rmt_set_pin(channel, RMT_MODE_TX, io_pin);
        rmt_set_mem_block_num(channel, 1);
        rmt_set_clk_div(channel, clock_divider);
        rmt_set_tx_loop_mode(channel, false);
        rmt_set_tx_carrier(channel, false, 0, 0, RMT_CARRIER_LEVEL_LOW);
        rmt_set_idle_level(channel, false, RMT_IDLE_LEVEL_LOW);
        rmt_set_source_clk(channel, RMT_BASECLK_APB);

        // Prepare high and low data
        auto t = timing.get_low();

        low.level0 = t.level0;
        low.duration0 = get_rmt_duration(t.duration0);
        low.level1 = t.level1;
        low.duration1 = get_rmt_duration(t.duration1);

        t = timing.get_high();
        hi.level0 = t.level0;
        hi.duration0 = get_rmt_duration(t.duration0);
        hi.level1 = t.level1;
        hi.duration1 = get_rmt_duration(t.duration1);

        t = timing.get_reset();
        reset.level0 = t.level0;
        reset.duration0 = get_rmt_duration(t.duration0);
        reset.level1 = t.level1;
        reset.duration1 = get_rmt_duration(t.duration1);

        // Reserve space for bits_per_pixel for each LED plus one for reset pulse.
        auto size = led_count * bits_per_pixel + 1;
        led_data.reserve(size);

        for (auto i = 0; i < size; ++i)
        {
            led_data.push_back(reset);
        }
    }

    RGBLed::~RGBLed()
    {
        rmt_driver_uninstall(channel);
    }

    uint16_t RGBLed::get_rmt_duration(uint16_t duration_in_ns)
    {
        return static_cast<uint16_t>(duration_in_ns / (pulse_width * clock_divider));
    }

    void RGBLed::set_pixel(uint16_t ix, uint8_t red, uint8_t green, uint8_t blue)
    {
        if (ix < led_count)
        {
            auto pixel = led_data.begin() + ix * bits_per_pixel;
            add_color(pixel, green);
            add_color(pixel, red);
            add_color(pixel, blue);
        }
    }

    void RGBLed::add_color(std::vector<rmt_item32_t>::iterator& pixel, uint8_t color)
    {
        core::util::ByteSet b(color);
        int bit = 7;

        while (bit >= 0 && pixel != led_data.end())
        {
            *pixel = b.test(bit) ? hi : low;
            bit--;
            pixel++;
        }
    }

    void RGBLed::clear()
    {
        for (auto i = 0; i < led_count; ++i)
        {
            set_pixel(i, 0, 0, 0);
        }
    }

    void RGBLed::apply()
    {
        // Be sure that the last item we send is the reset-pulse.
        led_data[led_data.size() - 1] = reset;
        rmt_write_items(channel, led_data.data(), led_data.size(), true);
    }
}
