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
#include "smooth/application/io/i2c/BME280.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core;
using namespace smooth::core::logging;

namespace smooth::application::sensor
{
    static const char* TAG = "BME280I2C";

    BME280::BME280(i2c_port_t port, uint8_t address, std::mutex& guard)
            : I2CMasterDevice(port, address, guard)
    {
    }

    uint8_t BME280::read_id()
    {
        core::util::FixedBuffer<uint8_t, 1> data;
        auto res = read(address, BME280Core::ID_REG, data);

        return res ? data[0] : 0;
    }

    bool BME280::reset()
    {
        // Write the magic number to the reset register
        std::vector<uint8_t> data{ BME280Core::RESET_REG, 0xB6 };

        return write(address, data, true);
    }

    bool BME280::read_status(bool& is_measuring, bool& updating_from_nvm)
    {
        core::util::FixedBuffer<uint8_t, 1> status_reg;

        bool res = read(address, BME280Core::STATUS_REG, status_reg);

        if (res)
        {
            is_measuring = status_reg[0] & 0x4;
            updating_from_nvm = status_reg[0] & 0x1;
        }

        return res;
    }

    bool BME280::configure_sensor(BME280Core::SensorMode mode,
                                  BME280Core::OverSampling humidity,
                                  BME280Core::OverSampling pressure,
                                  BME280Core::OverSampling temperature,
                                  BME280Core::StandbyTimeMS standby,
                                  BME280Core::FilterCoeff filter_coeff,
                                  BME280Core::SpiInterface spi_interface)
    {
        std::vector<uint8_t> datagram;

        bme280_core.get_configure_sensor_datagram(datagram,
                                                  mode,
                                                  humidity,
                                                  pressure,
                                                  temperature,
                                                  standby,
                                                  filter_coeff,
                                                  spi_interface);

        return write(address, datagram, true);
    }

    bool BME280::read_configuration(BME280Core::SensorMode& mode,
                                    BME280Core::OverSampling& humidity,
                                    BME280Core::OverSampling& pressure,
                                    BME280Core::OverSampling& temperature,
                                    BME280Core::StandbyTimeMS& standby,
                                    BME280Core::FilterCoeff& filter,
                                    BME280Core::SpiInterface& spi_interface)
    {
        core::util::FixedBuffer<uint8_t, 1> hum;
        core::util::FixedBuffer<uint8_t, 1> rest;
        core::util::FixedBuffer<uint8_t, 1> config;

        auto res = read(address, BME280Core::CTRL_HUM_REG, hum)
                   && read(address, BME280Core::CTRL_MEAS_REG, rest)
                   && read(address, BME280Core::CONFIG_REG, config);

        if (res)
        {
            bme280_core.get_configuration_settings(hum[0],
                                                   rest[0],
                                                   config[0],
                                                   mode,
                                                   humidity,
                                                   pressure,
                                                   temperature,
                                                   standby,
                                                   filter,
                                                   spi_interface);
        }

        return res;
    }

    // Read the trimming parameters
    bool BME280::read_trimming_parameters()
    {
        if (!trimming_read)
        {
            core::util::FixedBuffer<uint8_t, 32> calibration_data;
            core::util::FixedBuffer<uint8_t, 26> calib00_calib25_data;    // 0x88-0xA1
            core::util::FixedBuffer<uint8_t, 7> calib26_calib32_data;     // 0xE1-0xE7

            trimming_read = read(address, BME280Core::CALIB00_REG, calib00_calib25_data)
                            && read(address, BME280Core::CALIB26_REG, calib26_calib32_data);

            if (trimming_read)
            {
                // copy 0x88-0x9F into calibration_data
                for (size_t i = 0; i < 24; i++)
                {
                    calibration_data[i] = calib00_calib25_data[i];
                }

                // copy 0xA1 but not 0xA0 into calibration_data
                calibration_data[24] = calib00_calib25_data[25];

                // copy 0xE1 to 0xE7 into calibration_data
                for (size_t i = 0; i < 7; i++)
                {
                    calibration_data[i + 25] = calib26_calib32_data[i];
                }

                bme280_core.populate_trimming_registers(calibration_data);
            }
            else
            {
                Log::error(TAG, "Problems reading trimming parameters");
            }
        }

        return trimming_read;
    }

    // Read Temperature, Pressure and Humidity
    bool BME280::read_measurements(float& humidity, float& pressure, float& temperature)
    {
        core::util::FixedBuffer<uint8_t, 8> measurement_data{};

        humidity = 0;
        pressure = 0;
        temperature = 0;

        bool res = read_trimming_parameters()
                   && read(address, BME280Core::PRESS_MSB_REG, measurement_data);

        if (res)
        {
            bme280_core.get_measurements(measurement_data, humidity, pressure, temperature);
        }

        return res;
    }
}
