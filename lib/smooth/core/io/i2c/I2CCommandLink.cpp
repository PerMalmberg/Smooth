// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <smooth/core/io/i2c/I2CCommandLink.h>
#include <smooth/core/io/i2c/I2CMasterDevice.h>

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

