//
// Created by permal on 9/7/17.
//

#pragma once

#include <driver/rmt.h>
#include <vector>
#include "RGBLedTiming.h"

namespace smooth
{
    namespace application
    {
        namespace rgb_led
        {
            class RGBLed
            {
                public:
                    RGBLed(rmt_channel_t channel, gpio_num_t io_pin, uint16_t led_count, const RGBLedTiming& timing);

                    void set_pixel(uint16_t ix, uint8_t red, uint8_t green, uint8_t blue);
                    void apply();

                    virtual ~RGBLed();

                private:
                    static constexpr double pulse_width = 12.5; // ns, i.e. 1 / 80MHz

                    static constexpr uint16_t t0h = 400; // ns
                    static constexpr uint16_t t0l = 850; // ns
                    static constexpr uint16_t t1h = 800; // ns
                    static constexpr uint16_t t1l = 450; // ns
                    static constexpr uint16_t reset_time = 55000; // ns = 50us

                    static constexpr uint8_t clock_divider = 4;
                    static constexpr uint8_t bits_per_pixel = 24;

                    uint16_t get_rmt_duration(uint16_t duration_in_ns);
                    void add_color(std::vector<rmt_item32_t>::iterator& pixel, uint8_t color);


                    std::vector<rmt_item32_t> led_data;
                    rmt_channel_t channel;
                    uint8_t led_count;
                    rmt_item32_t hi;
                    rmt_item32_t low;
                    rmt_item32_t reset;
            };
        }
    }
}