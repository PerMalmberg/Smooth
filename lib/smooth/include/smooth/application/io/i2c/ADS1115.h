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

#pragma once

#include <chrono>
#include "smooth/core/io/i2c/I2CMasterDevice.h"

namespace smooth::application::io
{
    class ADS1115
        : public smooth::core::io::i2c::I2CMasterDevice
    {
        public:
            enum Register
            {
                Conversion = 0,
                Config = 1,
                LowThresh = 2,
                HighThresh = 3
            };

            enum Multiplexer
            {
                Diff_AIN0_AIN1 = 0,
                Diff_AIN0_AIN3 = 1,
                Diff_AIN1_AIN3 = 2,
                Diff_AIN2_AIN3 = 3,
                Single_AIN0 = 4,
                Single_AIN1 = 5,
                Single_AIN2 = 6,
                Single_AIN3 = 7
            };

            enum Range //FSR = Full Scale Range
            {
                // MSB:
                FSR_6_144 = 0,  // 187.5uV
                FSR_4_096 = 1,  // 125uV
                FSR_2_048 = 2,  // 62.5uV
                FSR_1_024 = 3,  // 31.25uV
                FSR_0_512 = 4,  // 15.625uV
                FSR_0_256 = 5   // 7.8125uV
            };

            enum OperationalMode
            {
                Continuous = 0,
                SingleShot = 1
            };

            enum DataRate
            : uint8_t
            {
                SPS_8 = 0,
                SPS_16 = 1,
                SPS_32 = 2,
                SPS_64 = 3,
                SPS_128 = 4,
                SPS_250 = 5,
                SPS_475 = 6,
                SPS_860 = 7
            };

            enum ComparatorMode
            {
                Traditional = 0,
                Window = 1
            };

            enum Alert_Ready_Polarity
            {
                ActiveLow = 0,
                ActiveHigh
            };

            enum LatchingComparator
            {
                NonLatching = 0,
                Latching = 1
            };

            enum AssertStrategy
            {
                AssertAfterOneConversion = 0,
                AssertAfterTwoConversions = 1,
                AssertAfterFourConversion = 2,
                DisableAssertion = 3,
            };

            ADS1115(i2c_port_t port, uint8_t address, std::mutex& guard);

            bool configure(Multiplexer mux,
                           Range range,
                           OperationalMode op_mode,
                           DataRate rate,
                           ComparatorMode comp_mode,
                           Alert_Ready_Polarity alert_ready_polarity,
                           LatchingComparator latching,
                           AssertStrategy assert_strategy,
                           uint16_t low_thresh_hold,
                           uint16_t high_thresh_hold);

            bool set_mux(const Multiplexer mux);

            bool read_conversion(uint16_t& result);

            bool trigger_single_read();

        private:
            bool configure(const uint16_t config,
                           uint16_t low_thresh_hold,
                           uint16_t high_thresh_hold);

            std::chrono::milliseconds minimum_delay_after_reconfigure() const;

            uint16_t current_config;
            uint16_t current_low_thresh_hold;
            uint16_t current_high_thresh_hold;
            std::chrono::steady_clock::time_point change_mark{ std::chrono::steady_clock::time_point::min() };
    };
}
