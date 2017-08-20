//
// Created by permal on 8/19/17.
//

#pragma once

#include <array>
#include <smooth/core/io/i2c/I2CMasterDevice.h>

namespace smooth
{
    namespace application
    {
        namespace sensor
        {
            // https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf
            class BME280
                    : public core::io::i2c::I2CMasterDevice
            {
                public:
                    BME280(i2c_port_t port, core::ipc::Mutex& guard, uint8_t address1);

                    uint8_t read_id();

                    enum SensorMode
                    {
                        Sleep = 0x00,
                        Forced = 0x01,
                        Normal = 0x02
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

                    bool configure_sensor(SensorMode mode, OverSampling humidity, OverSampling pressure, OverSampling temperature);
                    bool read_configuration(SensorMode& mode, OverSampling& humidity, OverSampling& pressure, OverSampling& temperature);


                private:
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
                    const uint8_t CAL_41_REG = 0xF0;
                    const uint8_t CAL_26_REG = 0xE1;
                    const uint8_t RESET_REG = 0xE0;
                    const uint8_t ID_REG = 0xD0;
                    const uint8_t CAL_25_REG = 0xA1;
                    const uint8_t CAL_00_REG = 0x88;
                    uint8_t address;
            };
        }
    }
}

