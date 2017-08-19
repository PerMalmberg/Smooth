//
// Created by permal on 8/19/17.
//


#include <vector>
#include <sstream>
#include <smooth/application/sensor/BMP280.h>
#include "esp_log.h"

namespace smooth
{
    namespace application
    {
        namespace sensor
        {
            BMP280::BMP280(i2c_port_t port, core::ipc::Mutex& guard)
                    : I2CMasterDevice(port, guard)
            {
            }

            void BMP280::read_id()
            {
                std::vector<uint8_t> data;
                data.reserve(1);
                data.push_back(0);

                ESP_LOGV("BMP280", "Reading");

                auto res = read(0x76, 0xD0, data);

                std::stringstream ss;
                for(auto b : data)
                {
                    ss << std::hex << static_cast<int>(b);
                }

                ESP_LOGV("BMP280", "ID: %s", ss.str().c_str());
                ESP_LOGV("BMP280", "Done: %d", res)
            }

        }
    }
}

