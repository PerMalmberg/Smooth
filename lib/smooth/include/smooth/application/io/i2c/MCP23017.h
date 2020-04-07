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

#pragma once

#include <utility>
#include "smooth/core/io/i2c/I2CMasterDevice.h"
#include "smooth/core/util/FixedBuffer.h"

namespace smooth::application::io
{
    // http://ww1.microchip.com/downloads/en/DeviceDoc/20001952C.pdf
    /// MCP23017 I2C I/O extender.
    /// @note This class assumes IOCON.BANK = 0 and IOCON.SEQOP = 0
    class MCP23017
        : public core::io::i2c::I2CMasterDevice
    {
        private:
            /// Register addresses used when IOCON.BANK = 0
            enum Register_BANK0
            : uint8_t
            {
                B0_IODIRA = 0x00,
                B0_IODIRB = 0x01,
                B0_IPOLA = 0x02,
                B0_IPOLB = 0x03,
                B0_GPINTENA = 0x04,
                B0_GPINTENB = 0x05,
                B0_DEFVALA = 0x06,
                B0_DEFVALB = 0x07,
                B0_INTCONA = 0x08,
                B0_INTCONB = 0x09,
                B0_IOCON = 0x0A,

                //B0_IOCON = 0x0B, // Second location
                B0_GPPUA = 0x0C,
                B0_GPPUB = 0x0D,
                B0_INTFA = 0x0E,
                B0_INTFB = 0x0F,
                B0_INTCAPA = 0x10,
                B0_INTCAPB = 0x11,
                B0_GPIOA = 0x12,
                B0_GPIOB = 0x13,
                B0_OLATA = 0x14,
                B0_OLATB = 0x15
            };

            /// Register addresses used when IOCON.BANK = 1
            enum Register_BANK1
            : uint8_t
            {
                B1_IODIRA = 0x00,
                B1_IPOLA = 0x01,
                B1_GPINTENA = 0x02,
                B1_DEFVALA = 0x03,
                B1_INTCONA = 0x04,
                B1_IOCON = 0x05,
                B1_GPPUA = 0x06,
                B1_INTFA = 0x07,
                B1_INTCAPA = 0x08,
                B1_GPIOA = 0x09,
                B1_OLATA = 0x0A,
                B1_IODIRB = 0x10,
                B1_IPOLB = 0x11,
                B1_GPINTENB = 0x12,
                B1_DEFVALB = 0x13,
                B1_INTCONB = 0x14,

                //B1_IOCON = 0x15, // Second location
                B1_GPPUB = 0x16,
                B1_INTFB = 0x17,
                B1_INTCAPB = 0x18,
                B1_GPIOB = 0x19,
                B1_OLATB = 0x1A,
            };

        public:
            enum Port
            {
                A,
                B
            };

            MCP23017(i2c_port_t port, uint8_t address, std::mutex& guard);

            /// Puts the device into a known state where IOCON.BANK = 0
            /// @note Unless you have specifically set IOCON.BANK=1, there is no need to call this method
            /// as the device defaults to IOCON.BANK = 0 on POR.
            /// \param enable_portb_int7 Set true if PORTB.INT7 should be enabled afterwards
            /// \return true on success, false on failure
            bool put_device_into_known_state(bool enable_portb_int7);

            /// Configures the I/O ports.
            /// \param port_a_direction A bit mask where 0 is output, 1 is input, for port A.
            /// \param input_a_pullup For I/O configured as input, enable internal pull-up if the corresponding bit is
            // set.
            /// \param input_a_polarity For I/O configured as input, report the inverted read state if the corresponding
            // bit is set. For port A.
            /// \param port_b_direction A bit mask where 0 is output, 1 is input, for port B.
            /// \param input_b_pullup For I/O that is configured as input, enable internal pull-up if the corresponding
            // bit is set.
            /// \param input_b_polarity For I/O configured as input, report the inverted read state if the corresponding
            // bit is set. For port B.
            /// \return true on success, false on failure
            bool configure_ports(uint8_t port_a_direction,
                                 uint8_t input_a_pull_up,
                                 uint8_t input_a_polarity,
                                 uint8_t port_b_direction,
                                 uint8_t input_b_pull_up,
                                 uint8_t input_b_polarity);

            /// Configures the device.
            /// @note Only a subset if the capabilities of the device can be configured, i.e those supported by this
            // implementation.
            /// \param mirror_change_interrupt If true, input change interrupts are logically OR:ed, resulting
            /// in both interrupts signaling on any change. If false, INTA is associated with PORTA and INTB with PORTB.
            /// \param interrupt_polarity_active_high If true, interrupt signaling is active-high.
            /// \param interrupt_on_change_enable_port_a bit mask for enabling interrupt-on-change on per pin-basis for
            // port A.
            /// \param interrupt_control_register_a For port A, controls how the associated pin value is compared for
            // the interrupt-on-change feature.
            /// If a bit is set, the corresponding I/O pin is compared against the associated bit in the
            // interrupt_default_val_a value. If a
            /// bit value is clear, the corresponding I/O pin is compared against the previous value.
            /// \param interrupt_default_val_a For port A, default compare register for interrupt on-change. If enabled
            // (via GPINTEN and
            /// INTCON) to compare against the DEFVAL register, as opposite value on the associated pin will cause an
            // interrupt to occur.
            /// \param interrupt_on_change_enable_port_b bit mask for enabling interrupt-on-change on per pin-basis for
            // port A.
            /// \param interrupt_control_register_b For port B, controls how the associated pin value is compared for
            // the interrupt-on-change feature.
            /// If a bit is set, the corresponding I/O pin is compared against the associated bit in the
            // interrupt_default_val_b value. If a
            /// bit value is clear, the corresponding I/O pin is compared against the previous value.
            /// \param interrupt_default_val_b For port B, default compare register for interrupt on-change. If enabled
            // (via GPINTEN and
            /// INTCON) to compare against the DEFVAL register, an opposite value on the associated pin will cause an
            // interrupt to occur.
            /// \return true on success, false on failure.
            bool configure_device(bool mirror_change_interrupt,
                                  bool interrupt_polarity_active_high,
                                  uint8_t interrupt_on_change_enable_a,
                                  uint8_t interrupt_control_register_a,
                                  uint8_t interrupt_default_val_a,
                                  uint8_t interrupt_on_change_enable_b,
                                  uint8_t interrupt_control_register_b,
                                  uint8_t interrupt_default_val_b);

            /// Sets the output state.
            /// \param port The port.
            /// \param state The state, where 1 is on, 0 is off.
            /// \return true on success, false on failure.
            bool set_output(Port port, uint8_t state);

            /// Reads the current output state
            /// \param port The port
            /// \param state The read state, where 1 is on, 0 is off
            /// \return true on success, false on failure
            bool read_output(Port port, uint8_t& state);

            /// Reads the inputs.
            /// \param port The port.
            /// \param state The receiver of the state of the inputs, where a 1 is logic-high.
            /// \return true on success, false on failure.
            bool read_input(Port port, uint8_t& state);

            /// Reads the interrupt capture filter. This register captures the input port state  at
            /// the time the interrupt occurred.
            /// @note This register remains unchanged until it is read, or the GPIO state is read.
            /// \param port The port for which to read the captured input value.
            /// \param state The value of the port when the interrupt triggered.
            /// \return true on success, false on failure
            bool read_interrupt_capture(Port port, uint8_t& state);

        private:
    };
}
