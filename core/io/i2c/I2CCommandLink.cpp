//
// Created by permal on 8/19/17.
//

#include <smooth/core/io/i2c/I2CCommandLink.h>
#include <smooth/core/io/i2c/I2CMasterDevice.h>

namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace i2c
            {
                I2CCommandLink::I2CCommandLink(I2CMasterDevice& master_device)
                        : master_device(master_device)
                {
                    // Acquire the guard to ensure noone else is trying to use the I2C bus.
                    master_device.get_guard().acquire();

                    cmd_link = i2c_cmd_link_create();
                }

                I2CCommandLink::~I2CCommandLink()
                {
                    i2c_cmd_link_delete(cmd_link);

                    // Release the guard
                    master_device.get_guard().release();
                }
            }
        }
    }
}