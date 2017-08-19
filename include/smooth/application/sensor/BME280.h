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
                    BME280(i2c_port_t port, core::ipc::Mutex& guard, uint8_t address);

                    uint8_t read_id();
                private:
                    uint8_t address;
            };
        }
    }
}

