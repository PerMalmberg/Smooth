//
// Created by permal on 8/19/17.
//


#include <vector>
#include <sstream>
#include <smooth/application/sensor/BME280.h>
#include <smooth/core/util/FixedBuffer.h>
#include <smooth/core/util/ByteSet.h>
#include "esp_log.h"

namespace smooth
{
    namespace application
    {
        namespace sensor
        {
            BME280::BME280(i2c_port_t port, core::ipc::Mutex& guard, uint8_t address)
                    : I2CMasterDevice(port, guard), address(address)
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
                std::vector<uint8_t> data{RESET_REG, 0xB6};
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
                uint8_t meas_data = (temperature << 5);
                meas_data |= (pressure << 2);
                meas_data |= mode;
                data.push_back(meas_data);

                data.push_back(CONFIG_REG);
                uint8_t config_data = standby << 5;
                config_data |= filter_coeff << 2;
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
                    trimming_read =
                            read_16bit(DIG_T1_REG, trimming.dig_T1)
                            && read_16bit(DIG_T2_REG, trimming.dig_T2)
                            && read_16bit(DIG_T3_REG, trimming.dig_T3);
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
                    uint32_t temp = measurement[5] & 0x0F;
                    temp |= measurement[4] << 4;
                    temp |= measurement[3] << (4 + 8);

                    temperature = BME280_compensate_T_int32(temp) / 100.0;
                }

                return res;
            }

            BME280::BME280_S32_t BME280::BME280_compensate_T_int32(BME280_S32_t adc_T)
            {
                BME280_S32_t var1, var2, T;
                var1 = ((((adc_T >> 3) - ((BME280_S32_t) trimming.dig_T1 << 1)))
                        * ((BME280_S32_t) trimming.dig_T2)) >> 11;

                var2 = (((((adc_T >> 4) - ((BME280_S32_t) trimming.dig_T1)) *
                          ((adc_T >> 4) - ((BME280_S32_t) trimming.dig_T1))) >> 12) *
                        ((BME280_S32_t) trimming.dig_T3)) >> 14;
                t_fine = var1 + var2;
                T = (t_fine * 5 + 128) >> 8;
                return T;
            }
/*
            BME280_U32_t BME280::BME280_compensate_P_int64(BME280_S32_t adc_P)
            {
                BME280_S64_t var1, var2, p;
                var1 = ((BME280_S64_t) t_fine) - 128000;
                var2 = var1 * var1 * (BME280_S64_t) dig_P6;
                var2 = var2 + ((var1 * (BME280_S64_t) dig_P5) << 17);
                var2 = var2 + (((BME280_S64_t) dig_P4) << 35);
                var1 = ((var1 * var1 * (BME280_S64_t) dig_P3) >> 8) + ((var1 * (BME280_S64_t) dig_P2) << 12);
                var1 = (((((BME280_S64_t) 1) << 47) + var1)) * ((BME280_S64_t) dig_P1) >> 33;
                if (var1 == 0)
                {
                    return 0; // avoid exception caused by division by zero
                }
                p = 1048576 - adc_P;
                p = (((p << 31) - var2) * 3125) / var1;
                var1 = (((BME280_S64_t) dig_P9) * (p >> 13) * (p >> 13)) >> 25;
                var2 = (((BME280_S64_t) dig_P8) * p) >> 19;
                p = ((p + var1 + var2) >> 8) + (((BME280_S64_t) dig_P7) << 4);
                return (BME280_U32_t) p;
            }

            BME280_U32_t BME280::bme280_compensate_H_int32(BME280_S32_t adc_H)
            {
                BME280_S32_t v_x1_u32r;
                v_x1_u32r = (t_fine - ((BME280_S32_t) 76800));
                v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t) dig_H4) << 20) - (((BME280_S32_t) dig_H5) *
                                                                                   v_x1_u32r)) +
                               ((BME280_S32_t) 16384)) >> 15) * (
                                     ((((((v_x1_u32r * ((BME280_S32_t) dig_H6)) >> 10) * (((v_x1_u32r *
                                                                                            ((BME280_S32_t) dig_H3))
                                             >> 11) +
                                                                                          ((BME280_S32_t) 32768)))
                                             >> 10) +
                                       ((BME280_S32_t) 2097152)) *
                                      ((BME280_S32_t) dig_H2) + 8192) >> 14));
                v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t) dig_H1))
                        >> 4));
                v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
                v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
                return (BME280_U32_t) (v_x1_u32r >> 12);
            }
*/
            /*
                Register Address Register content Data type
                0x88 / 0x89 dig_T1 [7:0] / [15:8] unsigned short
                0x8A / 0x8B dig_T2 [7:0] / [15:8] signed short
                0x8C / 0x8D dig_T3 [7:0] / [15:8] signed short
                0x8E / 0x8F dig_P1 [7:0] / [15:8] unsigned short
                0x90 / 0x91 dig_P2 [7:0] / [15:8] signed short
                0x92 / 0x93 dig_P3 [7:0] / [15:8] signed short
                0x94 / 0x95 dig_P4 [7:0] / [15:8] signed short
                0x96 / 0x97 dig_P5 [7:0] / [15:8] signed short
                0x98 / 0x99 dig_P6 [7:0] / [15:8] signed short
                0x9A / 0x9B dig_P7 [7:0] / [15:8] signed short
                0x9C / 0x9D dig_P8 [7:0] / [15:8] signed short
                0x9E / 0x9F dig_P9 [7:0] / [15:8] signed short
                0xA1 dig_H1 [7:0] unsigned char
                0xE1 / 0xE2 dig_H2 [7:0] / [15:8] signed short
                0xE3 dig_H3 [7:0] unsigned char
                0xE4 / 0xE5[3:0] dig_H4 [11:4] / [3:0] signed short
                0xE5[7:4] / 0xE6 dig_H5 [3:0] / [11:4] signed short
                0xE7 dig_H6 signed char

             */

        }
    }
}

