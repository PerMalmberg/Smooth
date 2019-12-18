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

#include <vector>
#include "smooth/core/util/FixedBuffer.h"

namespace smooth::application::sensor
{
    // https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf
    class BME280Core
    {
        public:
            static constexpr uint8_t HUM_LSB_REG = 0xFE;
            static constexpr uint8_t HUM_MSB_REG = 0xFD;
            static constexpr uint8_t TEMP_XLSB_REG = 0xFC;
            static constexpr uint8_t TEMP_LSB_REG = 0xFB;
            static constexpr uint8_t TEMP_MSB_REG = 0xFA;
            static constexpr uint8_t PRESS_XLSB_REG = 0xF9;
            static constexpr uint8_t PRESS_LSB_REG = 0xF8;
            static constexpr uint8_t PRESS_MSB_REG = 0xF7;
            static constexpr uint8_t CONFIG_REG = 0xF5;
            static constexpr uint8_t CTRL_MEAS_REG = 0xF4;
            static constexpr uint8_t STATUS_REG = 0xF3;
            static constexpr uint8_t CTRL_HUM_REG = 0xF2;
            static constexpr uint8_t RESET_REG = 0xE0;
            static constexpr uint8_t ID_REG = 0xD0;
            static constexpr uint8_t CALIB00_REG = 0x88;
            static constexpr uint8_t CALIB25_REG = 0xA1;
            static constexpr uint8_t CALIB26_REG = 0xE1;

            enum SensorMode
            {
                Sleep = 0x00,
                Forced = 0x01,
                Normal = 0x03
            };

            static const constexpr char* SensorModeStrings[] = { "Sleep",
                                                                 "Forced",
                                                                 "Forced",
                                                                 "Normal"
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

            static const constexpr char* OverSamplingStrings[] = { "Skipped",
                                                                   "Oversamplingx1",
                                                                   "Oversamplingx2",
                                                                   "Oversamplingx4",
                                                                   "Oversamplingx8",
                                                                   "Oversamplingx16"
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

            static const constexpr char* StandbyTimesStrings[] = { "ST_0_5",
                                                                   "ST_26_5",
                                                                   "ST_125",
                                                                   "ST_250",
                                                                   "ST_500",
                                                                   "ST_1000",
                                                                   "ST_10",
                                                                   "ST_20" }
            ;

            enum FilterCoeff
            {
                FC_OFF = 0x00,
                FC_2 = 0x01,
                FC_4 = 0x02,
                FC_58 = 0x03,
                FC_16 = 0x04
            };

            static const constexpr char* FilterCoeffStrings[] = { "FC_OFF",
                                                                  "FC_2",
                                                                  "FC_4",
                                                                  "FC_58",
                                                                  "FC_16"
            };

            enum SpiInterface
            {
                SPI_4_WIRE = 0x00,
                SPI_3_WIRE = 0x01
            };

            static const constexpr char* SpiInterfaceStrings[] = { "SPI_4_WIRE",
                                                                   "SPI_3_WIRE"
            };

            BME280Core() = default;

            /// Get a datagram that contains register addresses and data that will be used to configure the BME280
            /// \param datagram A vector that holds the configuration register addresses and data
            /// \param mode The mode setting
            /// \param humidity The humidity oversampling setting
            /// \param pressure The pressure oversampling setting
            /// \param temperature The temperature oversampling setting
            /// \param standby The standby time setting
            /// \param filter_coeff The filter coefficient setting
            /// \param spi_interface The spi interface setting
            void get_configure_sensor_datagram(std::vector<uint8_t>& datagram,
                                               SensorMode mode,
                                               OverSampling humidity,
                                               OverSampling pressure,
                                               OverSampling temperature,
                                               StandbyTimeMS standby,
                                               FilterCoeff filter_coeff,
                                               SpiInterface spi_interface);

            /// Get the current configuration settings
            /// \param ctrl_hum_data Data read from register 0xF2
            /// \param ctrl_meas_data Data read from register 0xF4
            /// \param config_data Data read from register 0xF5
            /// \param mode The mode configuration setting will be written to this address
            /// \param humidity The humidity configuration setting will be written to this address
            /// \param pressure The pressure configuration setting will be written to this address
            /// \param temperature The temperature configuration setting will be written to this address
            /// \param standby The standby times configuration setting will be written to this address
            /// \param filter_coeff The filter coefficient configuration setting will be written to this address
            /// \param spi_interface The spi_interface configuration setting will be written to this address
            void get_configuration_settings(uint8_t ctrl_hum_data,
                                            uint8_t ctrl_meas_data,
                                            uint8_t config_data,
                                            SensorMode& mode,
                                            OverSampling& humidity,
                                            OverSampling& pressure,
                                            OverSampling& temperature,
                                            StandbyTimeMS& standby,
                                            FilterCoeff& filter,
                                            SpiInterface& spi_interface);

            /// Get the compensated measurements
            /// \param measurment_data A container that holds the data read from measurement registers 0xF7..0xFE
            /// \param humidity Compensated Humidity, in %RH
            /// \param pressure Compenstated Pressure, in Pa
            /// \param temperature Compensated Temperature in degree Celsius
            //void get_measurements(std::array<uint8_t, 8>& measurement_data,
            void get_measurements(core::util::FixedBuffer<uint8_t, 8>& measurement_data,
                                  float& humidity,
                                  float& pressure,
                                  float& temperature);

            /// Populate the trimming registers for later use when calculating the compensated temp, press and hum
            // values
            /// \param calibration_data A container that holds the data read from calibration registers 0x88..0x9F,
            // 0xA1, 0xE1..0xE7.
            void populate_trimming_registers(core::util::FixedBuffer<uint8_t, 32>& calibration_data);

        private:
            using BME280_S32_t = int32_t;
            using BME280_U32_t = uint32_t;
            using BME280_S64_t = int64_t;

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

            // Structure that contains the trimming registers.  These registers are used in the compensation formulas
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
    };
}
