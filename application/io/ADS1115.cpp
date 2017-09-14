//
// Created by permal on 8/19/17.
//


#include <smooth/application/io/ADS1115.h>
#include <smooth/core/util/FixedBuffer.h>
#include "esp_log.h"

namespace smooth
{
    namespace application
    {
        namespace io
        {
            ADS1115::ADS1115(i2c_port_t port, uint8_t address, core::ipc::Mutex& guard)
                    : I2CMasterDevice(port, address, guard), current_config(0)
            {
            }

            bool ADS1115::configure(Multiplexer mux,
                                    Range range,
                                    OperationalMode op_mode,
                                    DataRate rate,
                                    ComparatorMode comp_mode,
                                    Alert_Ready_Polarity alert_ready_polarity,
                                    LatchingComparator latching,
                                    AssertStrategy assert_strategy)
            {

                uint16_t new_config{0};

                new_config |= mux << 12;
                new_config |= range << 9;
                new_config |= op_mode << 8;
                new_config |= rate << 5;
                new_config |= comp_mode << 4;
                new_config |= alert_ready_polarity << 3;
                new_config |= latching << 2;
                new_config |= assert_strategy;


                return configure(new_config);
            }

            bool ADS1115::configure(const uint16_t config)
            {
                std::vector<uint8_t> data{Register::Config};
                data.push_back(static_cast<uint8_t>(config >> 8));
                data.push_back(static_cast<uint8_t>(config & 0xFF));
                bool res = write(address, data, true);

                // Read back value to confirm write.
                core::util::FixedBuffer<uint8_t, 2> read_data;
                res = res && read(address, Register::Config, read_data, false, false);

                res = res && (read_data[1] == (config & 0xFF));
                res = res && (read_data[0] = (config >> 8));

                if (res)
                {
                    current_config = config;
                }

                return res;
            }

            bool ADS1115::set_mux(const Multiplexer mux)
            {
                uint16_t new_config = current_config | mux << 12;

                return configure(new_config);
            }

            bool ADS1115::read_conversion(uint16_t& result)
            {
                core::util::FixedBuffer<uint8_t, 2> data;
                bool res = read(address, Register::Conversion, data, false, false);

                if( res)
                {
                    result = 0;
                    result |= data[0] << 8;
                    result |= data[1];
                }

                return res;
            }
        }
    }
}

