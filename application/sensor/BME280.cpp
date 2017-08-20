//
// Created by permal on 8/19/17.
//


#include <vector>
#include <sstream>
#include <smooth/application/sensor/BME280.h>
#include <smooth/core/util/FixedBuffer.h>
#include <smooth/core/util/ByteSet.h>
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
                core::util::FixedBuffer<uint8_t, 1> data;
                auto res = read(address, ID_REG, data);
                return res ? data[0] : 0;
            }

            bool BME280::configure_sensor(SensorMode mode,
                                          OverSampling humidity,
                                          OverSampling pressure,
                                          OverSampling temperature)
            {
                std::vector<uint8_t> data;

                data.push_back(CTRL_HUM_REG);
                data.push_back(humidity);

                data.push_back(CTRL_MEAS_REG);
                uint8_t meas_data = (temperature << 5);
                meas_data |= (pressure << 2);
                meas_data |= mode;
                data.push_back(meas_data);

                return write(address, data, true);
            }

            bool BME280::read_configuration(SensorMode& mode,
                                            OverSampling& humidity,
                                            OverSampling& pressure,
                                            OverSampling& temperature)
            {
                core::util::FixedBuffer<uint8_t, 1> hum;
                core::util::FixedBuffer<uint8_t, 1> rest;

                bool res = read(address, CTRL_HUM_REG, hum);
                res &= read(address, CTRL_MEAS_REG, rest);

                if (res)
                {
                    core::util::ByteSet h(hum[0]);
                    humidity = static_cast<OverSampling>(h.get_bits_as_byte(0, 2));

                    core::util::ByteSet r(rest[0]);
                    mode = static_cast<SensorMode>(r.get_bits_as_byte(0, 1));
                    pressure = static_cast<OverSampling>(r.get_bits_as_byte(2, 4));
                    temperature = static_cast<OverSampling>(r.get_bits_as_byte(5, 7));
                }
                return res;
            }

        }
    }
}

