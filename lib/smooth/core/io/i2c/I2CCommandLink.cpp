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

#include "smooth/core/io/i2c/I2CCommandLink.h"
#include "smooth/core/io/i2c/I2CMasterDevice.h"

namespace smooth::core::io::i2c
{
    I2CCommandLink::I2CCommandLink(const I2CMasterDevice& master_device)
            : master_device(master_device)
    {
        // Acquire the guard to ensure noone else is trying to use the I2C bus.
        master_device.get_guard().lock();

        cmd_link = i2c_cmd_link_create();
    }

    I2CCommandLink::~I2CCommandLink()
    {
        i2c_cmd_link_delete(cmd_link);

        // Release the guard
        master_device.get_guard().unlock();
    }

    void I2CCommandLink::reset()
    {
        i2c_cmd_link_delete(cmd_link);
        cmd_link = i2c_cmd_link_create();
    }
}
