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
#include <vector>
#include "smooth/application/io/i2c/DHT12.h"

using namespace smooth::core;

namespace smooth::application::sensor
{
    // Constructor
    DHT12::DHT12(i2c_port_t port, uint8_t address, std::mutex& guard)
            : I2CMasterDevice(port, address, guard)
    {
    }

    // Read the raw data register from the DHT12, the last reagister is the checksum byte
    bool DHT12::read_raw_registers(core::util::FixedBuffer<uint8_t, 5>& raw_data)
    {
        return read(address, HumidityWholePart, raw_data);
    }

    // Is the checksum valid
    bool DHT12::is_checksum_valid(core::util::FixedBuffer<uint8_t, 5>& raw_data)
    {
        uint8_t sum = static_cast<uint8_t>(raw_data[HumidityWholePart] + raw_data[HumidityFractionalPart]
                                           + raw_data[TemperatureWholePart] + raw_data[TemperatureFractionalPart]);

        return raw_data[Checksum] == sum;
    }

    // Write device select; this write is required before we can read the data from the DHT12
    bool DHT12::write_device_select()
    {
        std::vector<uint8_t> data{ HumidityWholePart };

        return write(address, data, true);
    }

    // Read humidity and temperature
    bool DHT12::read_measurements(float& humidity, float& temperature)
    {
        core::util::FixedBuffer<uint8_t, 5> raw_data{};

        bool res = write_device_select() && read_raw_registers(raw_data) && is_checksum_valid(raw_data);

        // if res is OK then convert whole and fractional parts to floats
        if (res)
        {
            humidity = static_cast<float>(raw_data[HumidityWholePart])
                       + static_cast<float>(raw_data[HumidityFractionalPart] * 0.1);

            temperature = static_cast<float>(raw_data[TemperatureWholePart])
                          + static_cast<float>((raw_data[TemperatureFractionalPart] & 0x0F) * 0.1);

            if (raw_data[TemperatureFractionalPart] & 0x80)
            {
                temperature *= -1;
            }
        }

        return res;
    }
}
