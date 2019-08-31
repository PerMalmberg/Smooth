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

#include <bitset>
#include <thread>
#include <cmath>
#include <unordered_map>
#include <smooth/application/io/i2c/ADS1115.h>
#include <smooth/core/util/ByteSet.h>
#include <smooth/core/util/FixedBuffer.h>

using namespace std::chrono;

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
                std::vector<uint8_t> data{ Register::Config };
                data.push_back(static_cast<uint8_t>(config >> 8));
                data.push_back(static_cast<uint8_t>(config & 0xFF));
                bool res = write(address, data);

                // Read back value to confirm write.
                core::util::FixedBuffer<uint8_t, 2> read_data;
                res = res && read(address, Register::Config, read_data);

                // Need to filter bit 15 since it is changed by the device itself
                res = res && ((read_data[0] & 0x7F) == ((config >> 8) & 0x7F));
                res = res && (read_data[1] == (config & 0xFF));

                data.clear();
                data.push_back(Register::LowThresh);
                data.push_back(static_cast<uint8_t>(low_thresh_hold >> 8));
                data.push_back(static_cast<uint8_t>(low_thresh_hold & 0xFF));
                res = res && write(address, data);

                data.clear();
                data.push_back(Register::HighThresh);
                data.push_back(static_cast<uint8_t>(high_thresh_hold >> 8));
                data.push_back(static_cast<uint8_t>(high_thresh_hold & 0xFF));
                res = res && write(address, data);

                if (res)
                {
                    current_config = config;
                    current_low_thresh_hold = low_thresh_hold;
                    current_high_thresh_hold = high_thresh_hold;
                }

                change_mark = steady_clock::now();

                return res;
            }

            bool ADS1115::set_mux(const Multiplexer mux)
            {
                auto new_config = static_cast<uint16_t>(((current_config & 0x0FFF) | (mux << 12)));

                return configure(new_config, current_low_thresh_hold, current_high_thresh_hold);
            }

            bool ADS1115::read_conversion(uint16_t& result)
            {
                // Ensure that the device has had enough time to perform a conversion.
                auto delay = minimum_delay_after_reconfigure();

                if (steady_clock::now() <= change_mark + delay)
                {
                    std::this_thread::sleep_until(change_mark + delay);
                }

                core::util::FixedBuffer<uint8_t, 2> data;
                bool res = read(address, Register::Conversion, data);

                if (res)
                {
                    result = static_cast<uint16_t>(data[0] << 8);
                    result = static_cast<uint16_t>(result | data[1]);
                }

                return res;
            }

            bool ADS1115::trigger_single_read()
            {
                auto new_config = static_cast<uint16_t>(current_config | 1 << 15);

                return configure(new_config, current_low_thresh_hold, current_low_thresh_hold);
            }

            std::chrono::milliseconds ADS1115::minimum_delay_after_reconfigure() const
            {
                constexpr std::array<std::pair<DataRate, milliseconds>, 8> delays = {
                    std::make_pair(DataRate::SPS_8, milliseconds{ static_cast<int>(lround(1000.0 / 8)) }),
                    std::make_pair(DataRate::SPS_16, milliseconds{ static_cast<int>(lround(1000.0 / 16)) }),
                    std::make_pair(DataRate::SPS_32, milliseconds{ static_cast<int>(lround(1000.0 / 32)) }),
                    std::make_pair(DataRate::SPS_64, milliseconds{ static_cast<int>(lround(1000.0 / 64)) }),
                    std::make_pair(DataRate::SPS_128, milliseconds{ static_cast<int>(lround(1000.0 / 128)) }),
                    std::make_pair(DataRate::SPS_250, milliseconds{ static_cast<int>(lround(1000.0 / 250)) }),
                    std::make_pair(DataRate::SPS_475, milliseconds{ static_cast<int>(lround(1000.0 / 475)) }),
                    std::make_pair(DataRate::SPS_860, milliseconds{ static_cast<int>(lround(1000.0 / 860)) }),
                };

                auto sps = static_cast<DataRate>((current_config & 0x0070) >> 4);

                milliseconds delay{};

                auto found = std::find_if(std::begin(delays), std::end(delays), [sps](auto& pair)
                                          {
                                              return pair.first == sps;
                });

                if (found != std::end(delays))
                {
                    delay = (*found).second;
                }
                else
                {
                    delay = milliseconds{ 130 };
                }

                return delay;
            }
        }
    }
}
