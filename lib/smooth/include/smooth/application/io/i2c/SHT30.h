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
    class SHT30 : public smooth::core::io::i2c::I2CMasterDevice
    {
        public:
            enum Command : uint16_t
            {
                ReadStatus = 0xF32D,
                ClearStatus = 0x3041,
                DoSoftReset = 0x30A2,
                EnableHeater = 0x306D,
                DisableHeater = 0x3066
            };

            enum Repeatabilty
            {
                EnableMeasureHighRepeatabilityWithClockStretching,
                EnableMeasureMediumRepeatabilityWithClockStretching,
                EnableMeasureLowRepeatabilityWithClockStretching,
                DisableMeasureHighRepeatabilityWithClockStretching,
                DisableMeasureMediumRepeatabilityWithClockStretching,
                DisableMeasureLowRepeatabilityWithClockStretching,
            };

            /// Constructor
            SHT30(i2c_port_t port, uint8_t address, std::mutex& guard);

            /// Reads the temperature and humidity
            /// \param humidity The address to store the result from reading the humidity from SHT30
            /// \param temperature The address to store the result from reading the temperature from SHT30
            /// \return Return true on success, false on failure.
            bool read_measurements(float& humidity, float& temperature);

            /// Read staus register
            /// \param status The address to store the results from reading the status register
            /// \param return Return true if write was successful, false if failure
            bool read_status(uint16_t& status);

            /// Clear status
            /// \param return Return true on success, false on failure
            bool clear_status();

            /// Soft Reset
            /// \param return Return true on success, false on failure
            bool soft_reset();

            /// Turn heater ON
            /// \param return Return true on success, false on failure
            bool turn_heater_on();

            /// Turn heater OFF
            /// \param return Return true on success, false on failure
            bool turn_heater_off();

            /// Set repeatability mode
            /// \param mode The repeatability mode you want the SHT30 to use for measurements.
            ///             The default mode is MeasureHighRepeatabilityWithClockStretchingEnabled.
            void set_repeatability_mode(Repeatabilty mode);

            /// Get repeatability mode
            /// \param return Return the repeatability mode being used.
            std::string get_repeatability_mode();

        private:
            /// Calculate CRC
            /// \param raw_data The address of the FixedBuffer that contains data for caculting the CRC
            /// \param length The number of bytes in raw data to calculate the CRC on
            /// \return Return the calculated CRC vale
            uint8_t calculate_crc(smooth::core::util::FixedBuffer<uint8_t, 3>& raw_data, uint8_t length);

            /// Write command
            /// \param cmd The command to write to the SHT30
            /// \param return Return true on success, false on failure
            bool write_command(uint16_t cmd);

            /// Execute the write command
            /// \param cmd The command to write to the SHT30
            /// \param return Return true on success, false on failure
            bool execute_write_command(Command cmd);

            /// Execute the repeatability command
            /// \param return Return true on success, false on failure
            bool execute_repeatability_command();

            /// Execute the read block
            /// \param raw_data The address to store the result from reading the block
            /// \param return Return true on success, false on failure
            bool execute_read_block(smooth::core::util::FixedBufferBase<uint8_t>& raw_data);

            /// Execute crc checking on raw data
            /// \param raw_data The address of the data that will be used to check for CRC error
            /// \param length The number of bytes to do crc checking on
            /// \param crc_in_raw_data The crc value received from SHT30 in the raw_data
            /// \param return Return true on success, false on failure
            bool execute_crc_checking(smooth::core::util::FixedBuffer<uint8_t, 3>& raw_data, uint8_t length);

            Repeatabilty repeatability_command{ Repeatabilty::EnableMeasureHighRepeatabilityWithClockStretching };
    };
}
