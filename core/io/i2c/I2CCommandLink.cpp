//
// Created by permal on 8/19/17.
//

#include <smooth/core/io/i2c/I2CCommandLink.h>

namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace i2c
            {
                I2CCommandLink::I2CCommandLink()
                {
                    cmd_link = i2c_cmd_link_create();
                }

                I2CCommandLink::~I2CCommandLink()
                {
                    i2c_cmd_link_delete(cmd_link);
                }
            }
        }
    }
}