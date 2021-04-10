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

#include <thread>
#include <algorithm>
#include "smooth/application/io/i2c/SHT30.h"
#include "smooth/core/logging/log.h"

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::util;
using namespace smooth::core::logging;

namespace smooth::application::sensor
{
    // Class constants
    static const char* TAG = "SHT30";

    constexpr const milliseconds command_delay_2ms(2);
    constexpr const milliseconds non_clock_stretching_measurement_delay_16ms(16);
    constexpr const int scl_timeout_7ms = 560000;  // (560000 / 80MHz) = 0.007 = 7ms
    constexpr const uint16_t repeatability_command[] = { 0x2C06, 0x2C0D, 0x2C10, 0x2400, 0x240B, 0x2416 };

    constexpr const char* repeatability_name[] = { "MeasureHighRepeatabilityWithClockStretchingEnabled",
                                                   "MeasureMediumRepeatabilityWithClockStretchingEnabled",
                                                   "MeasureLowRepeatabilityWithClockStretchingEnabled",
                                                   "MeasureHighRepeatabilityWithClockStretchingDisabled",
                                                   "MeasureMediumRepeatabilityWithClockStretchingDisabled",
                                                   "MeasureLowRepeatabilityWithClockStretchingDisabled" };

    // Constructor
    SHT30::SHT30(i2c_port_t port, uint8_t address, std::mutex& guard) : I2CMasterDevice(port, address, guard)
    {
    }

    // Read humidity and temperature measurements
    bool SHT30::read_measurements(float& humidity, float& temperature)
    {
        util::FixedBuffer<uint8_t, 6> raw_data{};
        util::FixedBuffer<uint8_t, 3> temp_raw_data{};
        util::FixedBuffer<uint8_t, 3> humd_raw_data{};

        bool res = false;

        if (execute_measurement_read(raw_data))
        {
            std::copy(raw_data.begin(), raw_data.begin() + 3, temp_raw_data.begin());
            std::copy(raw_data.begin() + 3, raw_data.end(), humd_raw_data.begin());

            if (execute_crc_checking(temp_raw_data, 2) && execute_crc_checking(humd_raw_data, 2))
            {
                uint16_t raw_temp = static_cast<uint16_t>((raw_data[0] << 8) + raw_data[1]);
                uint16_t raw_humd = static_cast<uint16_t>((raw_data[3] << 8) + raw_data[4]);

                // convert raw values into real values
                temperature = static_cast<float>(-45.0 + (175.0 * raw_temp / 65535.0));
                humidity = static_cast<float>(100.0 * (raw_humd / 65535.0));

                res = true;
            }
        }

        return res;
    }

    // Read status register
    bool SHT30::read_status(uint16_t& status)
    {
        bool res = false;
        util::FixedBuffer<uint8_t, 3> raw_data{};

        if (execute_status_read(raw_data) && execute_crc_checking(raw_data, 2))
        {
            status = static_cast<uint16_t>((raw_data[0] << 8) + raw_data[1]);
            res = true;
        }

        return res;
    }

    // Clear the status register on the SHT30
    bool SHT30::clear_status()
    {
        return execute_write_command(Command::ClearStatus);
    }

    // Perform a soft reset on the SHT30
    bool SHT30::soft_reset()
    {
        return execute_write_command(Command::DoSoftReset);
    }

    // Turn heater ON
    bool SHT30::turn_heater_on()
    {
        return execute_write_command(Command::EnableHeater);
    }

    // Turn heater OFF
    bool SHT30::turn_heater_off()
    {
        return execute_write_command(Command::DisableHeater);
    }

    // Set repeatability mode
    void SHT30::set_repeatability_mode(RepeatabilityMode mode)
    {
        // We will only use clock stretching on Medium and Low Repeatability Measurements
        // since the ESP32 maximum i2c-master-timeout is 13ms.  If user selects
        // EnableMeasureHighRepeatabilityWithClockStretching it will be implemented using
        // MeasureHighRepeatabilityWithClockStretchingDisabled
        if ((mode == RepeatabilityMode::EnableMeasureMediumRepeatabilityWithClockStretching)
            || (mode == RepeatabilityMode::EnableMeasureLowRepeatabilityWithClockStretching))
        {
            use_clock_stretching = true;
        }
        else
        {
            use_clock_stretching = false;
        }

        repeatability_mode = mode;
    }

    // Get repeatability mode
    std::string SHT30::get_repeatability_mode()
    {
        return repeatability_name[repeatability_mode];
    }

    // Write command is required before we can read the data from the SHT30
    bool SHT30::write_command(uint16_t cmd)
    {
        std::vector<uint8_t> command;
        command.push_back(static_cast<uint8_t>(cmd >> 8));
        command.push_back(static_cast<uint8_t>(cmd & 0xFF));
        bool res = write(address, command, true);

        return res;
    }

    // Calculate the CRC
    uint8_t SHT30::calculate_crc(FixedBuffer<uint8_t, 3>& raw_data, uint8_t length)
    {
        // Polynomial: 0x31 (x8 + x5 + x4 + 1)
        // Initialization: 0xFF
        // Final XOR: 0x00
        // Example: CRC (0xBEEF) = 0x92

        const uint8_t CRC8POLY = 0x31;
        const uint8_t CRC8INIT = 0xFF;
        uint8_t calc_crc = CRC8INIT;

        for (uint8_t index = 0; index < length; index++)
        {
            calc_crc = static_cast<uint8_t>(calc_crc ^ raw_data[index]);

            // for each bit postion in byte
            for (uint8_t bits_left = 8; bits_left > 0; bits_left--)
            {
                calc_crc = (calc_crc & 0x80)
                ? CRC8POLY ^ static_cast<uint8_t>(calc_crc << 1)
                : static_cast<uint8_t>(calc_crc << 1);
            }
        }

        return calc_crc;
    }

    // Execute the write command to SHT30
    bool SHT30::execute_write_command(Command cmd)
    {
        bool res = write_command(cmd);

        // required delay between sending commands
        std::this_thread::sleep_for(command_delay_2ms);

        if (!res)
        {
            Log::error(TAG, "Execute write command failed");
        }

        return res;
    }

    // Execute the repeatability command to SHT30
    bool SHT30::execute_repeatability_command()
    {
        bool res = write_command(repeatability_command[repeatability_mode]);

        // We set delay that can be used by all non-clock-stretching modes
        std::this_thread::sleep_for(non_clock_stretching_measurement_delay_16ms);

        if (!res)
        {
            Log::error(TAG, "Execute repeatability command failed");
        }

        return res;
    }

    // Execute CRC checking
    bool SHT30::execute_crc_checking(FixedBuffer<uint8_t, 3>& raw_data, uint8_t length)
    {
        // last byte in raw_data holds the crc value sent from SHT30
        bool res = calculate_crc(raw_data, length) == raw_data[2];

        if (!res)
        {
            Log::error(TAG, "Execute CRC check failed");
        }

        return res;
    }

    // Execute reading block of data from SHT30
    bool SHT30::execute_read_block(FixedBufferBase<uint8_t>& raw_data)
    {
        bool res = read_block(address, raw_data);

        if (!res)
        {
            Log::error(TAG, "Execute read_block failed");
        }

        return res;
    }

    // Execute read16 that uses slave clock-stretching to read data from SHT30
    bool SHT30::execute_read16(FixedBufferBase<uint8_t>& raw_data)
    {
        // Read the measurement data using slave clock-stretching.  The master_timeout set to 560000 = 7ms,
        // this timeout will satisfy both clock-stretching modes
        bool res = read16(address, repeatability_command[repeatability_mode], raw_data, true, true, scl_timeout_7ms);

        if (!res)
        {
            Log::error(TAG, "Execute read16 failed");
        }

        return res;
    }

    // Execute measurement read
    bool SHT30::execute_measurement_read(FixedBufferBase<uint8_t>& raw_data)
    {
        bool res = false;

        if (use_clock_stretching)
        {
            res = execute_read16(raw_data);
        }
        else
        {
            res = execute_repeatability_command() && execute_read_block(raw_data);
        }

        if (!res)
        {
            Log::error(TAG, "Execute measurement read failed");
        }

        return res;
    }

    // Execute status read
    bool SHT30::execute_status_read(FixedBufferBase<uint8_t>& raw_data)
    {
        auto res = execute_write_command(Command::ReadStatus) && execute_read_block(raw_data);

        if (!res)
        {
            Log::error(TAG, "Execute status read failed");
        }

        return res;
    }
}
