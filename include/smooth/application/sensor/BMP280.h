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
            class BMP280
                    : public core::io::i2c::I2CMasterDevice
            {
                public:
                    BMP280(i2c_port_t port, core::ipc::Mutex& guard);

                    void read_id();

            };
        }
    }
}

