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
#include <sstream>
#include <smooth/application/io/i2c/BME280.h>
#include <smooth/core/util/FixedBuffer.h>
#include <smooth/core/util/ByteSet.h>

namespace smooth::application::sensor
{
    BME280::BME280(i2c_port_t port, uint8_t address, std::mutex& guard)
            : I2CMasterDevice(port, address, guard)
    {
    }

    uint8_t BME280::read_id()
    {
        core::util::FixedBuffer<uint8_t, 1> data;
        auto res = read(address, ID_REG, data);

        return res ? data[0] : 0;
    }

    bool BME280::reset()
    {
        // Write the magic number to the reset register
        std::vector<uint8_t> data{ RESET_REG, 0xB6 };

        return write(address, data, true);
    }

    bool BME280::read_status(bool& is_measuring, bool& updating_from_nvm)
    {
        core::util::FixedBuffer<uint8_t, 1> status_reg;

        bool res = read(address, STATUS_REG, status_reg);

        if (res)
        {
            is_measuring = status_reg[0] & 0x4;
            updating_from_nvm = status_reg[0] & 0x1;
        }

        return res;
    }

    bool BME280::configure_sensor(SensorMode mode,
                                  OverSampling humidity,
                                  OverSampling pressure,
                                  OverSampling temperature,
                                  StandbyTimeMS standby,
                                  FilterCoeff filter_coeff)
    {
        std::vector<uint8_t> data;

        data.push_back(CTRL_HUM_REG);
        data.push_back(humidity);

        data.push_back(CTRL_MEAS_REG);
        auto meas_data = static_cast<uint8_t>(temperature << 5);
        meas_data = static_cast<uint8_t>(meas_data | (pressure << 2));
        meas_data = static_cast<uint8_t>(meas_data | mode);
        data.push_back(meas_data);

        data.push_back(CONFIG_REG);
        auto config_data = static_cast<uint8_t>((standby << 5) | (filter_coeff << 2));
        data.push_back(config_data);

        return write(address, data, true);
    }

    bool BME280::read_configuration(SensorMode& mode,
                                    OverSampling& humidity,
                                    OverSampling& pressure,
                                    OverSampling& temperature,
                                    StandbyTimeMS& standby,
                                    FilterCoeff& filter)
    {
        core::util::FixedBuffer<uint8_t, 1> hum;
        core::util::FixedBuffer<uint8_t, 1> rest;
        core::util::FixedBuffer<uint8_t, 1> config;

        bool res = read(address, CTRL_HUM_REG, hum);
        res &= read(address, CTRL_MEAS_REG, rest);
        res &= read(address, CONFIG_REG, config);

        if (res)
        {
            core::util::ByteSet h(hum[0]);
            humidity = static_cast<OverSampling>(h.get_bits_as_byte(0, 2));

            core::util::ByteSet r(rest[0]);
            mode = static_cast<SensorMode>(r.get_bits_as_byte(0, 1));
            pressure = static_cast<OverSampling>(r.get_bits_as_byte(2, 4));
            temperature = static_cast<OverSampling>(r.get_bits_as_byte(5, 7));

            core::util::ByteSet c(config[0]);
            standby = static_cast<StandbyTimeMS>(c.get_bits_as_byte(5, 7));
            filter = static_cast<FilterCoeff>(c.get_bits_as_byte(2, 4));
        }

        return res;
    }

    bool BME280::read_trimming_parameters()
    {
        if (!trimming_read)
        {
            uint8_t h4{};
            uint8_t h4_1{};
            uint8_t h5{};
            uint8_t h5_1{};

            trimming_read =
                read_16bit(DIG_T1_REG, trimming.dig_T1)
                && read_16bit(DIG_T2_REG, trimming.dig_T2)
                && read_16bit(DIG_T3_REG, trimming.dig_T3)
                && read_16bit(DIG_P1_REG, trimming.dig_P1)
                && read_16bit(DIG_P2_REG, trimming.dig_P2)
                && read_16bit(DIG_P3_REG, trimming.dig_P3)
                && read_16bit(DIG_P4_REG, trimming.dig_P4)
                && read_16bit(DIG_P5_REG, trimming.dig_P5)
                && read_16bit(DIG_P6_REG, trimming.dig_P6)
                && read_16bit(DIG_P7_REG, trimming.dig_P7)
                && read_16bit(DIG_P8_REG, trimming.dig_P8)
                && read_16bit(DIG_P9_REG, trimming.dig_P9)
                && read_8bit(DIG_H1_REG, trimming.dig_H1)
                && read_16bit(DIG_H2_REG, trimming.dig_H2)
                && read_8bit(DIG_H3_REG, trimming.dig_H3)
                && read_8bit(DIG_H6_REG, trimming.dig_H6)

                // The following data is a bit tricky as it is split over multiple bytes.
                // dig_H4: Bits 11:4 located in DIG_H4_REG, bits 3:0 located in DIG_H4_REG+1
                // dig_H5: bits 3:0 located in DIG_H5_REG[7:4], bits 11:4 in DIG_H5_REG+1

                && read_8bit(DIG_H4_REG, h4)
                && read_8bit(static_cast<uint8_t>(DIG_H4_REG + 1), h4_1)
                && read_8bit(DIG_H5_REG, h5)
                && read_8bit(static_cast<uint8_t>(DIG_H5_REG + 1), h5_1);

            // Adjust values so they are directly usable.
            trimming.dig_H4 = static_cast<int16_t>((h4 << 4) | (h4_1 & 0xF));
            trimming.dig_H5 = static_cast<int16_t>((h5 >> 4) | (h5_1 << 4));
        }

        return trimming_read;
    }

    bool BME280::read_measurements(float& humidity, float& pressure, float& temperature)
    {
        core::util::FixedBuffer<uint8_t, 8> measurement{};

        humidity = 0;
        pressure = 0;
        temperature = 0;

        bool res = read_trimming_parameters();
        res = res && read(address, PRESS_MSB_REG, measurement);

        if (res)
        {
            // Temperature data is at index: MSB: 3, LSB: 4, XLSB: 5
            auto temp = measurement[5] & 0x0F;
            temp |= measurement[4] << 4;
            temp |= measurement[3] << (4 + 8);

            temperature = static_cast<float>(BME280_compensate_T_int32(temp) / 100.0);

            // Pressure data is at index: MSB: 0, LSB: 1, XLSB: 2
            int32_t press = measurement[2] & 0xF;
            press |= measurement[1] << 4;
            press |= measurement[0] << (4 + 8);

            pressure = static_cast<float>(BME280_compensate_P_int64(press) / 256.0);

            // Humidity data is at index: MSB: 6, LSB: 7

            int32_t hum = measurement[7];
            hum |= measurement[6] << 8;

            humidity = static_cast<float>(BME280_compensate_H_int32(hum) / 1024);
        }

        return res;
    }

    BME280::BME280_S32_t BME280::BME280_compensate_T_int32(BME280_S32_t adc_T)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
        BME280_S32_t var1, var2, T;
        var1 = ((((adc_T >> 3) - ((BME280_S32_t)trimming.dig_T1 << 1)))
                * ((BME280_S32_t)trimming.dig_T2)) >> 11;

        var2 = (((((adc_T >> 4) - ((BME280_S32_t)trimming.dig_T1))
                  * ((adc_T >> 4) - ((BME280_S32_t)trimming.dig_T1))) >> 12)
                * ((BME280_S32_t)trimming.dig_T3)) >> 14;
        t_fine = var1 + var2;
        T = (t_fine * 5 + 128) >> 8;

        return T;
#pragma GCC diagnostic pop
    }

    BME280::BME280_U32_t BME280::BME280_compensate_P_int64(BME280_S32_t adc_P)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
        BME280_S64_t var1, var2, p;
        var1 = ((BME280_S64_t)t_fine) - 128000;
        var2 = var1 * var1 * (BME280_S64_t)trimming.dig_P6;
        var2 = var2 + ((var1 * (BME280_S64_t)trimming.dig_P5) << 17);
        var2 = var2 + (((BME280_S64_t)trimming.dig_P4) << 35);
        var1 = ((var1 * var1 * (BME280_S64_t)trimming.dig_P3) >> 8)
               + ((var1 * (BME280_S64_t)trimming.dig_P2) << 12);
        var1 = (((((BME280_S64_t)1) << 47) + var1)) * ((BME280_S64_t)trimming.dig_P1) >> 33;

        if (var1 == 0)
        {
            return 0;         // avoid exception caused by division by zero
        }

        p = 1048576 - adc_P;
        p = (((p << 31) - var2) * 3125) / var1;
        var1 = (((BME280_S64_t)trimming.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
        var2 = (((BME280_S64_t)trimming.dig_P8) * p) >> 19;
        p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)trimming.dig_P7) << 4);

        return (BME280_U32_t)p;
#pragma GCC diagnostic pop
    }

    BME280::BME280_U32_t BME280::BME280_compensate_H_int32(BME280_S32_t adc_H)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
        BME280_S32_t v_x1_u32r;
        v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
        v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)trimming.dig_H4) << 20)
                        - (((BME280_S32_t)trimming.dig_H5)
                           * v_x1_u32r))
                       + ((BME280_S32_t)16384)) >> 15) * (
                         ((((((v_x1_u32r * ((BME280_S32_t)trimming.dig_H6)) >> 10) * (((v_x1_u32r
                                                                                        * ((BME280_S32_t)
                                                                                           trimming.dig_H3))
                                                                                       >> 11)
                                                                                      + ((BME280_S32_t)32768)))
                            >> 10)
                           + ((BME280_S32_t)2097152))
                          * ((BME280_S32_t)trimming.dig_H2) + 8192) >> 14));
        v_x1_u32r = (v_x1_u32r
                     - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)trimming.dig_H1))
                        >> 4));
        v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
        v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

        return (BME280_U32_t)(v_x1_u32r >> 12);
#pragma GCC diagnostic pop
    }
}
