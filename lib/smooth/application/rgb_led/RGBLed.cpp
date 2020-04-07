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

#include "smooth/application/rgb_led/RGBLed.h"
#include "smooth/core/util/ByteSet.h"
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
