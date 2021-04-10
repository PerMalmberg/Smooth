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

/************************************************************************************
    SPECIAL NOTES                SPECIAL NOTES                          SPECIAL NOTES

    The SHT30 can be configured to operate in slave clock-stretching mode where the
    SHT30 holds the SCL line low while it is busy taking temperature and humdity
    measurements. The amount of time the SHT30 holds the SCL line low depends upon
    which repeatabilty mode is selected.

    For fastest response time but lowest repeatability the user should select the
    repeatability mode of EnableMeasureLowRepeatabilityWithClockStretching.

    For medium response time and with medium repeatability the user should select the
    repeatability mode of EnableMeasureMediumRepeatabilityWithClockStretching.  This
    is the default setting on the SHT30.

    For the longest response time but highest repeatability the user should select the
    repeatability mode of DisableMeasureHighRepeatabilityWithClockStretching.

    From the test I ran on the SHT30 at room temperature there was very little
    difference between the various modes (24.4099 Celsius to 24.5060 Celsius).

 ************************************************************************************/
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

            enum RepeatabilityMode
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
            ///             The default mode is MeasureHighRepeatabilityWithClockStretchingDisabled.
            void set_repeatability_mode(RepeatabilityMode mode);

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

            /// Execute the read block which does not use clock stretching
            /// \param raw_data The address to store the result from reading
            /// \param return Return true on success, false on failure
            bool execute_read_block(smooth::core::util::FixedBufferBase<uint8_t>& raw_data);

            /// Execute the read16 which uses clock stretching
            /// \param raw_data The address to store the result from reading
            /// \param return Return true on success, false on failure
            bool execute_read16(smooth::core::util::FixedBufferBase<uint8_t>& raw_data);

            /// Execute measuremnt read
            /// \param raw_data The address to store the result from reading the measuremnt
            /// \param return Return true on success, false on failure
            bool execute_measurement_read(smooth::core::util::FixedBufferBase<uint8_t>& raw_data);

            /// Execute status read
            /// \param raw_data The address to store the result from reading the status
            /// \param return Return true on success, false on failure
            bool execute_status_read(smooth::core::util::FixedBufferBase<uint8_t>& raw_data);

            /// Execute crc checking on raw data
            /// \param raw_data The address of the data that will be used to check for CRC error
            /// \param length The number of bytes to do crc checking on
            /// \param crc_in_raw_data The crc value received from SHT30 in the raw_data
            /// \param return Return true on success, false on failure
            bool execute_crc_checking(smooth::core::util::FixedBuffer<uint8_t, 3>& raw_data, uint8_t length);

            /// The default repeatability mode
            RepeatabilityMode repeatability_mode{ RepeatabilityMode::
                                                  DisableMeasureMediumRepeatabilityWithClockStretching };

            bool use_clock_stretching{ false };
    };
}
