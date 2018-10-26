//
// Created by permal on 8/19/17.
//

#pragma once

#include <driver/i2c.h>

namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace i2c
            {
                class I2CMasterDevice;

                class I2CCommandLink
                {
                    public:
                        I2CCommandLink(const I2CMasterDevice& master_device);
                        ~I2CCommandLink();

                        operator i2c_cmd_handle_t() const
                        {
                            return cmd_link;
                        }

                        void reset();

                    private:
                        i2c_cmd_handle_t cmd_link;
                        const I2CMasterDevice& master_device;
                };
            }
        }
    }
}