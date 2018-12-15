//
// Created by permal on 8/19/17.
//


#include <smooth/application/io/i2c/ADS1115.h>
#include <smooth/core/util/FixedBuffer.h>
#include <bitset>
#include "esp_log.h"

namespace smooth
{
    namespace application
    {
        namespace io
        {
            ADS1115::ADS1115(i2c_port_t port, uint8_t address, std::mutex& guard)
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
                                    AssertStrategy assert_strategy,
                                    uint16_t low_thresh_hold,
                                    uint16_t high_thresh_hold)
            {

                std::bitset<16> new_config;
                new_config.set(0, assert_strategy & 1);
                new_config.set(1, assert_strategy & 2);

                new_config.set(2, latching);
                new_config.set(3, alert_ready_polarity);
                new_config.set(4, comp_mode);

                new_config.set(5, rate & 1);
                new_config.set(6, rate & 2);
                new_config.set(7, rate & 4);

                new_config.set(8, op_mode);

                new_config.set(9, range & 1);
                new_config.set(10, range & 2);
                new_config.set(11, range & 4);

                new_config.set(12, mux & 1);
                new_config.set(13, mux & 2);
                new_config.set(14, mux & 4);

                new_config.set(15, 0);

                return configure(static_cast<uint16_t>(new_config.to_ulong()), low_thresh_hold, high_thresh_hold);
            }

            bool ADS1115::configure(const uint16_t config,
                                    uint16_t low_thresh_hold,
                                    uint16_t high_thresh_hold)
            {
                std::vector<uint8_t> data{Register::Config};
                data.push_back(static_cast<uint8_t>(config >> 8));
                data.push_back(static_cast<uint8_t>(config & 0xFF));
                bool res = write(address, data, true);

                // Read back value to confirm write.
                core::util::FixedBuffer<uint8_t, 2> read_data;
                res = res && read(address, Register::Config, read_data, false, false);

                res = res && (read_data[0] = (config >> 8));
                res = res && (read_data[1] == (config & 0xFF));

                data.clear();
                data.push_back(Register::LowThresh);
                data.push_back(low_thresh_hold >> 8);
                data.push_back(low_thresh_hold & 0xFF);
                res = res && write(address, data, true);

                data.clear();
                data.push_back(Register::HighThresh);
                data.push_back(high_thresh_hold >> 8);
                data.push_back(high_thresh_hold & 0xFF);
                res = res && write(address, data, true);

                if (res)
                {
                    current_config = config;
                    current_low_thresh_hold = low_thresh_hold;
                    current_high_thresh_hold = high_thresh_hold;
                }

                return res;
            }

            bool ADS1115::set_mux(const Multiplexer mux)
            {
                uint16_t new_config = (current_config & 0x0FFF) | mux << 12;

                return configure(new_config, current_low_thresh_hold, current_high_thresh_hold);
            }

            bool ADS1115::read_conversion(uint16_t& result)
            {
                core::util::FixedBuffer<uint8_t, 2> data;
                bool res = read(address, Register::Conversion, data, false, false);

                if (res)
                {
                    result = 0;
                    result |= data[0] << 8;
                    result |= data[1];
                }

                return res;
            }

            bool ADS1115::trigger_single_read()
            {
                uint16_t new_config = current_config | 1 << 15;

                return configure(new_config, current_low_thresh_hold, current_low_thresh_hold);
            }
        }
    }
}

