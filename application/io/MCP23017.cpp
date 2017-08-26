//
// Created by permal on 8/19/17.
//


#include <vector>
#include <sstream>
#include <smooth/application/io/MCP23017.h>
#include <smooth/core/util/FixedBuffer.h>
#include <smooth/core/util/ByteSet.h>
#include "esp_log.h"

namespace smooth
{
    namespace application
    {
        namespace io
        {
            MCP23017::MCP23017(i2c_port_t port, uint8_t address, core::ipc::Mutex& guard)
                    : I2CMasterDevice(port, address, guard)
            {
            }


            bool MCP23017::put_device_into_known_state(bool enable_portb_int7)
            {
                core::util::FixedBuffer<uint8_t, 1> read_data{};

                // Assume IOCON.BANK = 1
                bool res = read(address, B1_IOCON, read_data);
                // Clear BANK register (or if device is currently in BANK=0, GPINTENB.GPINT7)
                std::vector<uint8_t> data{B1_IOCON, static_cast<uint8_t>(read_data[0] & 0x7F)};
                res = res && write(address, data, true);
                // Device is now either in BANK=0, or we just disabled GPINTENB.GPINT7.
                if (enable_portb_int7)
                {
                    // Enable GPINTENB.GPINT7
                    res = res && read(address, B0_GPINTENB, read_data);
                    data = {B0_GPINTENB, static_cast<uint8_t>(read_data[0] | 0x80)};
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
                std::vector<uint8_t> data{B0_IODIRA, port_a_direction,
                                          port_b_direction,
                                          input_a_polarity,
                                          input_b_polarity};
                bool res = write(address, data, true);
                std::vector<uint8_t> pol{B0_GPPUA, input_a_polarity, input_b_polarity};
                res = res && write(address, pol, true);
                return res;
            }

            bool MCP23017::set_output(Port port, uint8_t state)
            {
                auto p = (port == Port::A) ? B0_OLATA : B0_OLATB;
                std::vector<uint8_t> data{p, state};
                return write(address, data, true);
            }
        }
    }
}

