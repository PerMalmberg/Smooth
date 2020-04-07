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

#include <utility>
#include "smooth/core/io/i2c/I2CMasterDevice.h"
#include "smooth/application/io/spi/BME280Core.h"
#include "smooth/core/util/FixedBuffer.h"

namespace smooth::application::sensor
{
    // https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf
    class BME280
        : public core::io::i2c::I2CMasterDevice
    {
        public:
            BME280(i2c_port_t port, uint8_t address, std::mutex& guard);

            /// Reads the ID of the device
            uint8_t read_id();

            /// Resets the device
            bool reset();

            /// Configures the sensor
            /// \param mode The mode
            /// \param humidity Humidity oversampling
            /// \param pressure Pressure oversampling
            /// \param temperature Temperature oversampling
            /// \return true on success, false on failure
            bool configure_sensor(BME280Core::SensorMode mode,
                                  BME280Core::OverSampling humidity,
                                  BME280Core::OverSampling pressure,
                                  BME280Core::OverSampling temperature,
                                  BME280Core::StandbyTimeMS standby,
                                  BME280Core::FilterCoeff filter_coeff,
                                  BME280Core::SpiInterface spi_interface = BME280Core::SpiInterface::SPI_4_WIRE);

            /// Reads the current configuration
            /// \param mode The mode
            /// \param humidity Humidity oversampling
            /// \param pressure Pressure oversampling
            /// \param temperature Temperature oversampling
            /// \return true on success, false on failure
            bool read_configuration(BME280Core::SensorMode& mode,
                                    BME280Core::OverSampling& humidity,
                                    BME280Core::OverSampling& pressure,
                                    BME280Core::OverSampling& temperature,
                                    BME280Core::StandbyTimeMS& standby,
                                    BME280Core::FilterCoeff& filter,
                                    BME280Core::SpiInterface& spi_interface);

            /// Reads the status of the device
            /// \param is_measuring true if the device is doing conversions.
            /// \param updating_from_nvm true if data is being copied from NVM to image registers.
            /// \return true if status could be read, otherwise false.
            bool read_status(bool& is_measuring, bool& updating_from_nvm);

            /// Reads the current measurements
            /// \param humidity Humidity, in %RH
            /// \param pressure Pressure, in Pa
            /// \param temperature Temperature in degree Celsius
            /// \return true on success, false on failure.
            bool read_measurements(float& humidity, float& pressure, float& temperature);

        private:
            template<typename T>
            bool read_16bit(uint8_t start_reg, T& target)
            {
                core::util::FixedBuffer<uint8_t, 2> buff;
                bool res = read(address, start_reg, buff);

                if (res)
                {
                    target = buff[0];
                    target = static_cast<T>(target | (buff[1] << 8));
                }

                return res;
            }

            template<typename T>
            bool read_8bit(uint8_t start_reg, T& target)
            {
                core::util::FixedBuffer<uint8_t, 1> buff;
                bool res = read(address, start_reg, buff);

                if (res)
                {
                    target = static_cast<T>(buff[0]);
                }

                return res;
            }

            struct TrimmingType
            {
                uint16_t dig_T1;
                int16_t dig_T2;
                int16_t dig_T3;

                uint16_t dig_P1;
                int16_t dig_P2;
                int16_t dig_P3;
                int16_t dig_P4;
                int16_t dig_P5;
                int16_t dig_P6;
                int16_t dig_P7;
                int16_t dig_P8;
                int16_t dig_P9;

                uint8_t dig_H1;
                int16_t dig_H2;
                uint8_t dig_H3;
                int16_t dig_H4; // 0xE4 / 0xE5[3:0] | [11:4] / [3:0]
                int16_t dig_H5; // 0xE5[7:4] / 0xE6 | [3:0] / [11:4]
                int8_t dig_H6;
            };

            TrimmingType trimming{};

            bool read_trimming_parameters();

            bool trimming_read = false;
            smooth::application::sensor::BME280Core bme280_core{};
    };
}
