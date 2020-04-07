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
#include "smooth/core/util/ByteSet.h"
#include "smooth/application/io/spi/BME280Core.h"

using namespace smooth::core;

namespace smooth::application::sensor
{
    // Get config_sensor datagram that contains register addresses and data that will be used to configure the BME280
    void BME280Core::get_configure_sensor_datagram(std::vector<uint8_t>& datagram,
                                                   SensorMode mode,
                                                   OverSampling humidity,
                                                   OverSampling pressure,
                                                   OverSampling temperature,
                                                   StandbyTimeMS standby,
                                                   FilterCoeff filter_coeff,
                                                   SpiInterface spi_interface)
    {
        // Register 0xF2
        datagram.push_back(CTRL_HUM_REG);
        datagram.push_back(humidity);

        // Register 0xF4
        datagram.push_back(CTRL_MEAS_REG);
        auto meas_data = temperature << 5;
        meas_data = meas_data | (pressure << 2);
        meas_data = (meas_data | mode);
        datagram.push_back(static_cast<uint8_t>(meas_data));

        // Register 0xF5
        datagram.push_back(CONFIG_REG);
        auto config_data = (standby << 5) | (filter_coeff << 2) | spi_interface;
        datagram.push_back(static_cast<uint8_t>(config_data));
    }

    // Get the current configuration of BME280
    void BME280Core::get_configuration_settings(uint8_t ctrl_hum_data,
                                                uint8_t ctrl_meas_data,
                                                uint8_t config_data,
                                                SensorMode& mode,
                                                OverSampling& humidity,
                                                OverSampling& pressure,
                                                OverSampling& temperature,
                                                StandbyTimeMS& standby,
                                                FilterCoeff& filter,
                                                SpiInterface& spi_interface)
    {
        util::ByteSet h(ctrl_hum_data);
        humidity = static_cast<OverSampling>(h.get_bits_as_byte(0, 2));

        util::ByteSet r(ctrl_meas_data);
        mode = static_cast<SensorMode>(r.get_bits_as_byte(0, 1));
        pressure = static_cast<OverSampling>(r.get_bits_as_byte(2, 4));
        temperature = static_cast<OverSampling>(r.get_bits_as_byte(5, 7));

        util::ByteSet c(config_data);
        standby = static_cast<StandbyTimeMS>(c.get_bits_as_byte(5, 7));
        filter = static_cast<FilterCoeff>(c.get_bits_as_byte(2, 4));
        spi_interface = static_cast<SpiInterface>(config_data & 0x01);
    }

    // Populate the trimming registers that will be used in calculation for compensated measurements
    void BME280Core::populate_trimming_registers(core::util::FixedBuffer<uint8_t, 32>& calibration_data)
    {
        trimming.dig_T1 = static_cast<uint16_t>(calibration_data[1] << 8 | calibration_data[0]);
        trimming.dig_T2 = static_cast<int16_t>(calibration_data[3] << 8 | calibration_data[2]);
        trimming.dig_T3 = static_cast<int16_t>(calibration_data[5] << 8 | calibration_data[4]);
        trimming.dig_P1 = static_cast<uint16_t>(calibration_data[7] << 8 | calibration_data[6]);
        trimming.dig_P2 = static_cast<int16_t>(calibration_data[9] << 8 | calibration_data[8]);
        trimming.dig_P3 = static_cast<int16_t>(calibration_data[11] << 8 | calibration_data[10]);
        trimming.dig_P4 = static_cast<int16_t>(calibration_data[13] << 8 | calibration_data[12]);
        trimming.dig_P5 = static_cast<int16_t>(calibration_data[15] << 8 | calibration_data[14]);
        trimming.dig_P6 = static_cast<int16_t>(calibration_data[17] << 8 | calibration_data[16]);
        trimming.dig_P7 = static_cast<int16_t>(calibration_data[19] << 8 | calibration_data[18]);
        trimming.dig_P8 = static_cast<int16_t>(calibration_data[21] << 8 | calibration_data[20]);
        trimming.dig_P9 = static_cast<int16_t>(calibration_data[23] << 8 | calibration_data[22]);
        trimming.dig_H1 = calibration_data[24];
        trimming.dig_H2 = static_cast<int16_t>(calibration_data[26] << 8 | calibration_data[25]);
        trimming.dig_H3 = calibration_data[27];
        trimming.dig_H4 = static_cast<int16_t>((calibration_data[28] << 4) | (calibration_data[29] & 0x0F));  // 0xE4<<4
                                                                                                              // | 0xE5&0x0F
        trimming.dig_H5 = static_cast<int16_t>((calibration_data[30] << 4) | (calibration_data[29] >> 4));    // 0xE6<<4
                                                                                                              // | 0xE5>>4
        trimming.dig_H6 = static_cast<int8_t>(calibration_data[31]);
    }

    // Get the compensated measurements
    void BME280Core::get_measurements(core::util::FixedBuffer<uint8_t, 8>& measurement_data,
                                      float& humidity, float& pressure, float& temperature)
    {
        // Temperature data is at index: MSB: 3, LSB: 4, XLSB: 5
        auto temp = measurement_data[5] & 0x0F;
        temp |= measurement_data[4] << 4;
        temp |= measurement_data[3] << (4 + 8);

        temperature = static_cast<float>(BME280_compensate_T_int32(temp) / 100.0);

        // Pressure data is at index: MSB: 0, LSB: 1, XLSB: 2
        int32_t press = measurement_data[2] & 0xF;
        press |= measurement_data[1] << 4;
        press |= measurement_data[0] << (4 + 8);

        pressure = static_cast<float>(BME280_compensate_P_int64(press) / 256.0);

        // Humidity data is at index: MSB: 6, LSB: 7
        int32_t hum = measurement_data[7];
        hum |= measurement_data[6] << 8;

        humidity = static_cast<float>(BME280_compensate_H_int32(hum) / 1024);
    }

    // The following calculation methods are based on those in the datasheet at
    // https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf
    // as of 2017-08-20. The revision of the code is 1.1 according to that document.

    // Compensation formula for TEMPERATURE
    BME280Core::BME280_S32_t BME280Core::BME280_compensate_T_int32(BME280_S32_t adc_T)
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

    // Compensation formula for PRESSURE
    BME280Core::BME280_U32_t BME280Core::BME280_compensate_P_int64(BME280_S32_t adc_P)
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

    // Compensation formula for HUMIDITY
    BME280Core::BME280_U32_t BME280Core::BME280_compensate_H_int32(BME280_S32_t adc_H)
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
