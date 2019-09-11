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
#include <smooth/core/io/i2c/I2CMasterDevice.h>
#include <smooth/core/util/FixedBuffer.h>

namespace smooth::application::sensor
{
    // https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf
    class BME280
        : public core::io::i2c::I2CMasterDevice
    {
        public:
            BME280(i2c_port_t port, uint8_t address, std::mutex& guard);

            enum SensorMode
            {
                Sleep = 0x00,
                Forced = 0x01,
                Normal = 0x03
            };

            enum OverSampling
            {
                Skipped = 0x00,
                Oversamplingx1 = 0x01,
                Oversamplingx2 = 0x02,
                Oversamplingx4 = 0x03,
                Oversamplingx8 = 0x04,
                Oversamplingx16 = 0x05
            };

            enum StandbyTimeMS
            {
                ST_0_5 = 0x00,
                ST_62_5 = 0x01,
                ST_125 = 0x02,
                ST_250 = 0x03,
                ST_500 = 0x04,
                ST_1000 = 0x05,
                ST_10 = 0x06,
                ST_20 = 0x07
            };

            enum FilterCoeff
            {
                FC_OFF = 0x00,
                FC_2 = 0x01,
                FC_4 = 0x02,
                FC_58 = 0x03,
                FC_16 = 0x04
            };

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
            bool configure_sensor(SensorMode mode,
                                  OverSampling humidity,
                                  OverSampling pressure,
                                  OverSampling temperature,
                                  StandbyTimeMS standby,
                                  FilterCoeff filter_coeff);

            /// Reads the current configuration
            /// \param mode The mode
            /// \param humidity Humidity oversampling
            /// \param pressure Pressure oversampling
            /// \param temperature Temperature oversampling
            /// \return true on success, false on failure
            bool read_configuration(SensorMode& mode,
                                    OverSampling& humidity,
                                    OverSampling& pressure,
                                    OverSampling& temperature,
                                    StandbyTimeMS& standby,
                                    FilterCoeff& filter);

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
            // These calculation methods are based on those in the datasheet at
            // https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf
            // as of 2017-08-20. The revision of the code is 1.1 according to that document.
            typedef int32_t BME280_S32_t;
            typedef uint32_t BME280_U32_t;
            typedef int64_t BME280_S64_t;

            BME280_S32_t t_fine;

            // Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
            // t_fine carries fine temperature as global value
            BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T);

            // Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional
            // bits).
            // Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
            BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P);

            // Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
            // Output value of “47445” represents 47445/1024 = 46.333 %RH
            BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H);

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
            }
            trimming;

            bool read_trimming_parameters();

            bool trimming_read = false;

            const uint8_t HUM_LSB_REG = 0xFE;
            const uint8_t HUM_MSB_REG = 0xFD;
            const uint8_t TEMP_XLSB_REG = 0xFC;
            const uint8_t TEMP_LSB_REG = 0xFB;
            const uint8_t TEMP_MSB_REG = 0xFA;
            const uint8_t PRESS_XLSB_REG = 0xF9;
            const uint8_t PRESS_LSB_REG = 0xF8;
            const uint8_t PRESS_MSB_REG = 0xF7;
            const uint8_t CONFIG_REG = 0xF5;
            const uint8_t CTRL_MEAS_REG = 0xF4;
            const uint8_t STATUS_REG = 0xF3;
            const uint8_t CTRL_HUM_REG = 0xF2;
            const uint8_t RESET_REG = 0xE0;
            const uint8_t ID_REG = 0xD0;

            const uint8_t DIG_T1_REG = 0x88;
            const uint8_t DIG_T2_REG = 0x8A;
            const uint8_t DIG_T3_REG = 0x8C;

            const uint8_t DIG_P1_REG = 0x8E;
            const uint8_t DIG_P2_REG = 0x90;
            const uint8_t DIG_P3_REG = 0x92;
            const uint8_t DIG_P4_REG = 0x94;
            const uint8_t DIG_P5_REG = 0x96;
            const uint8_t DIG_P6_REG = 0x98;
            const uint8_t DIG_P7_REG = 0x9A;
            const uint8_t DIG_P8_REG = 0x9C;
            const uint8_t DIG_P9_REG = 0x9E;

            const uint8_t DIG_H1_REG = 0xA1;
            const uint8_t DIG_H2_REG = 0xE1;
            const uint8_t DIG_H3_REG = 0xE2;
            const uint8_t DIG_H4_REG = 0xE4; // 0xE4 / 0xE5[3:0]
            const uint8_t DIG_H5_REG = 0xE5; // 0xE5[7:4] / 0xE6
            const uint8_t DIG_H6_REG = 0xE7;
    };
}
