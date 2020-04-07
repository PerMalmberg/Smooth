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

#include "smooth/core/io/i2c/I2CMasterDevice.h"
#include "smooth/core/util/FixedBuffer.h"

namespace smooth::application::sensor
{
    class DHT12
        : public core::io::i2c::I2CMasterDevice
    {
        public:
            enum Registers
            {
                HumidityWholePart = 0x00,
                HumidityFractionalPart,
                TemperatureWholePart,
                TemperatureFractionalPart,
                Checksum
            };

            DHT12(i2c_port_t port, uint8_t address, std::mutex& guard);

            /// Reads the temperature and humidity
            /// \param humidity The address to store the result from reading the humidity from the device
            /// \param temperature The address to store the result from reading the temperature from the device
            /// \return true on success, false on failure.
            bool read_measurements(float& humidity, float& temperature);

            /// Validate the checksum
            /// \param raw_data The address of the FixedBuffer that contains all the register data read from the DHT12
            /// \return Return true if checksum is valid, false if checksum is not valid
            bool is_checksum_valid(core::util::FixedBuffer<uint8_t, 5>& raw_data);

            /// Read the raw data registers from the DHT12
            /// \param raw_data The address of the FixedBuffer that will hold the register data read from the DHT12
            /// \param return Return true if read was successful, false if not
            bool read_raw_registers(core::util::FixedBuffer<uint8_t, 5>& raw_data);

            /// Write device select, write the starting register to begin reading from
            /// \param return Return true if write was successful, false if not
            bool write_device_select();

        private:
    };
}
