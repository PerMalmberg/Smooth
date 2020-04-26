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

#include <array>
#include <vector>
#include "smooth/core/io/Output.h"
#include "smooth/core/io/spi/Master.h"
#include "smooth/core/io/spi/SPIDevice.h"
#include "smooth/application/display/DisplayTypes.h"
#include "smooth/application/display/DisplayPin.h"

namespace smooth::application::display
{
    class LCDSpi : public core::io::spi::SPIDevice
    {
        public:
            /// The constructor
            LCDSpi(std::mutex& guard,
                   gpio_num_t chip_select_pin,
                   gpio_num_t data_command_pin,
                   uint8_t spi_command_bits,
                   uint8_t spi_address_bits,
                   uint8_t bits_between_address_and_data_phase,
                   uint8_t spi_mode,
                   uint8_t spi_positive_duty_cycle,
                   uint8_t spi_cs_ena_posttrans,
                   int spi_clock_speed_hz,
                   uint32_t spi_device_flags,
                   int spi_queue_size,
                   bool use_pre_transaction_callback,
                   bool use_post_transaction_callback);

            /// Initialize the display
            /// \param host The SPI host either VSPI or HSPI
            /// \return true on success, false on failure
            bool init(spi_host_device_t host);

            /// Set the MADCTL register to a value
            /// \param value The MADCTL value
            /// \param madctl_reg The MADCTL register address default set to 0x36
            /// \return true on success, false on failure
            bool set_madctl(uint8_t value, uint8_t madctl_reg = 0x36);

            /// Hardware Reset
            /// \param active_low If true the reset pin requires a LOW to force the chip to reset,
            /// if false the reset pin requires a HIGH to force the chip to reset.
            /// \param active_time The amount of time in milliseconds the pin will be in the active state
            /// \param delay_time The amount of time in milliseconds after exiting the active state,
            /// the pin is the non-active state (waiting for the chip to complete it's reset).
            void hw_reset(bool active_low, std::chrono::milliseconds active_time, std::chrono::milliseconds delay_time);

            /// Software reset
            /// \param delay_time The amount of time in milliseconds to delay.  Some ammount
            /// of delay time is required before one can send a new command.
            /// \return true on success, false on failure
            bool sw_reset(std::chrono::milliseconds delay_time);

            /// Send init commands - write multiple init commands to display
            /// Send a sequence of commands and data to the display to perform intialization
            /// \param init_cmds The pointer to the first byte in init_cmds
            /// \param length The number of bytes in the init_cmds
            /// \return true on success, false on failure
            //bool send_init_cmds(const display_init_cmd_t* init_cmds, size_t length);
            bool send_init_cmds(const DisplayInitCmd* init_cmds, size_t length);

            /// Send Commands - write multiple commands to display
            /// \param cmds The pointer to the first byte in the cmds
            /// \param length The number of bytes in the cmds
            /// \return true on success, false on failure
            bool send_cmds(const uint8_t* cmds, size_t length);

            /// Send Command - write single command to display
            /// \param cmd The command (register address) to write
            /// \return true on success, false on failure
            bool send_cmd(const uint8_t cmd);

            /// Send Data - write data to display
            /// \param data The pointer to the first byte in the data
            /// \param length The number of bytes in the data
            /// \return true on success, false on failure
            bool send_data(const uint8_t* data, size_t length);

            /// Send lines
            /// Sends color data to display starting at position x1,y1 and ending at position x2,y2
            /// \param x1 The top left corner x position
            /// \param y1 The top left corner y position
            /// \param x2 The bottom right corner x position
            /// \param y2 The bottom right corner y position
            /// \param data The pointer to the first byte in the data (color data)
            /// \param length The number of bytes in the data
            /// \return true on success, false on failure
            bool send_lines(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint8_t* data, size_t length);

            /// Wait for send lines to finish;
            /// Waits for all SPI transactions in send lines to complete
            /// \return true on success, false on failure
            bool wait_for_send_lines_to_finish();

            /// Read parameters
            /// \param cmd The command (address register)
            /// \param data The address of the container to hold the parameter data
            /// \param param_count The number of parameters to be read
            /// \return true on success, false on failure
            bool read_params(uint8_t cmd, std::vector<uint8_t>& data, uint32_t param_count);

            /// Read
            /// \param cmd The command or register address to read
            /// \param rxdata The pointer to the first byte location of the receive data container
            /// \param length The number of bytes to raed
            /// \return true on success, false on failure
            bool read(uint8_t cmd, uint8_t* rxdata, size_t length);

            /// Add reset pin
            /// \param reset_pin
            void add_reset_pin(std::unique_ptr<DisplayPin> reset_pin);

            /// add backlight pin
            /// \param bk_light_pin
            void add_backlight_pin(std::unique_ptr<DisplayPin> bk_light_pin);

            /// Set backlight
            /// Turn backlight either ON or OFF
            /// \param level True = PIN_HIGH, false = PIN_LOW
            void set_back_light(bool level);

        private:
            /// Prepare pins for command transaction
            /// \param trans_num The transaction number; 0-5
            void prepare_pins_for_cmd_transaction(size_t trans_num);

            /// Prepare pins for data transaction
            /// \param trans_num The transaction number; 0-5
            void prepare_pins_for_data_transaction(size_t trans_num);

            /// Pre Transmission Action
            /// The operations that need to be performed before the spi transaction is started
            void pre_transmission_action() override
            {
                dc_pin.set(dc_pretrans_pin_states.at(current_transaction));
                cs_pin.set(cs_pretrans_pin_states.at(current_transaction));
            }

            /// Post Transmission Action
            /// The operations that need to be performed after the spi transaction has ended
            void post_transmission_action() override
            {
                cs_pin.set(cs_posttrans_pin_states.at(current_transaction));
                current_transaction++;
            }

            smooth::core::io::Output dc_pin;
            smooth::core::io::Output cs_pin;
            std::unique_ptr<DisplayPin> backlight_pin{};
            std::unique_ptr<DisplayPin> reset_pin{};

            static constexpr int line_transaction_length = 6;
            using PreTransPinState = std::array<bool, line_transaction_length>;
            PreTransPinState dc_pretrans_pin_states{};
            PreTransPinState cs_pretrans_pin_states{};
            PreTransPinState cs_posttrans_pin_states{};
            std::size_t current_transaction{};
    };
}
