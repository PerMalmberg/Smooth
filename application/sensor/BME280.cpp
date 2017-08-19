//
// Created by permal on 8/19/17.
//


#include <vector>
#include <sstream>
#include <smooth/application/sensor/BME280.h>
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
                std::vector<uint8_t> data;
                data.reserve(1);
                data.push_back(0);
                auto res = read(address, 0xD0, data);
                return res ? data[0] : 0;
            }

        }
    }
}

