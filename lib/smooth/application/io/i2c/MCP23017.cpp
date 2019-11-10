/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <vector>
#include <sstream>
#include "smooth/application/io/i2c/MCP23017.h"
#include "smooth/core/util/FixedBuffer.h"
#include "smooth/core/util/ByteSet.h"

namespace smooth
{
    namespace application
    {
        namespace io
        {
            MCP23017::MCP23017(i2c_port_t port, uint8_t address, std::mutex& guard)
                    : I2CMasterDevice(port, address, guard)
            {
            }

            bool MCP23017::put_device_into_known_state(bool enable_portb_int7)
            {
                core::util::FixedBuffer<uint8_t, 1> read_data{};

                // Assume IOCON.BANK = 1
                bool res = read(address, B1_IOCON, read_data);

                // Clear BANK register (or if device is currently in BANK=0, GPINTENB.GPINT7)
                std::vector<uint8_t> data{ B1_IOCON, static_cast<uint8_t>(read_data[0] & 0x7F) };
                res = res && write(address, data, true);

                // Device is now either in BANK=0, or we just disabled GPINTENB.GPINT7.
                if (enable_portb_int7)
                {
                    // Enable GPINTENB.GPINT7
                    res = res && read(address, B0_GPINTENB, read_data);
                    data = { B0_GPINTENB, static_cast<uint8_t>(read_data[0] | 0x80) };
                    res = res && write(address, data, true);
                }

                return res;
            }

            bool MCP23017::configure_ports(uint8_t port_a_direction,
                                           uint8_t input_a_pull_up,
                                           uint8_t input_a_polarity,
                                           uint8_t port_b_direction,
                                           uint8_t input_b_pull_up,
                                           uint8_t input_b_polarity)
            {
                // Write direction and polarity in a single operation.
                std::vector<uint8_t> data{ B0_IODIRA,
                                           port_a_direction,
                                           port_b_direction,
                                           input_a_polarity,
                                           input_b_polarity };

                bool res = write(address, data, true);

                // Write pull-up
                std::vector<uint8_t> pull_up{ B0_GPPUA,
                                              input_a_pull_up,
                                              input_b_pull_up };

                res = res && write(address, pull_up, true);

                return res;
            }

            bool MCP23017::configure_device(bool mirror_change_interrupt,
                                            bool interrupt_polarity_active_high,
                                            uint8_t interrupt_on_change_enable_a,
                                            uint8_t interrupt_control_register_a,
                                            uint8_t interrupt_default_val_a,
                                            uint8_t interrupt_on_change_enable_b,
                                            uint8_t interrupt_control_register_b,
                                            uint8_t interrupt_default_val_b)
            {
                core::util::FixedBuffer<uint8_t, 1> data;

                // Read current config
                bool res = read(address, B0_IOCON, data);

                if (res)
                {
                    core::util::ByteSet b(data[0]);
                    b.set(6, mirror_change_interrupt);
                    b.set(1, interrupt_polarity_active_high);

                    std::vector<uint8_t> d{ B0_IOCON, b };
                    res = write(address, d, true);
                }

                if (res)
                {
                    // GPINTENA, GPINTENB, DEFVALA, DEFVALB, INTCONA, INTCONB
                    std::vector<uint8_t> d{ B0_GPINTENA,
                                            interrupt_on_change_enable_a,
                                            interrupt_on_change_enable_b,
                                            interrupt_default_val_a,
                                            interrupt_default_val_b,
                                            interrupt_control_register_a,
                                            interrupt_control_register_b };

                    res = write(address, d, true);
                }

                return res;
            }

            bool MCP23017::set_output(Port port, uint8_t state)
            {
                auto p = (port == Port::A) ? B0_OLATA : B0_OLATB;
                std::vector<uint8_t> data{ p, state };

                return write(address, data, true);
            }

            bool MCP23017::read_output(Port port, uint8_t& state)
            {
                // Read latches, not actual value on the outputs.
                auto p = (port == Port::A) ? B0_OLATA : B0_OLATB;
                core::util::FixedBuffer<uint8_t, 1> read_data{};
                auto res = read(address, p, read_data);

                if (res)
                {
                    state = read_data[0];
                }

                return res;
            }

            bool MCP23017::read_input(Port port, uint8_t& state)
            {
                auto p = (port == Port::A) ? B0_GPIOA : B0_GPIOB;
                core::util::FixedBuffer<uint8_t, 1> read_data{};
                auto res = read(address, p, read_data);

                if (res)
                {
                    state = read_data[0];
                }

                return res;
            }

            bool MCP23017::read_interrupt_capture(Port port, uint8_t& state)
            {
                core::util::FixedBuffer<uint8_t, 1> read_data{};
                bool res = read(address, port == Port::A ? B0_INTCAPA : B0_INTCAPB, read_data);

                if (res)
                {
                    state = read_data[0];
                }

                return res;
            }
        }
    }
}
