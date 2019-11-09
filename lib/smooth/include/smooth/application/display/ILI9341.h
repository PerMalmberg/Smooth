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

/************************************************************************************
   SPECIAL NOTES                SPECIAL NOTES                          SPECIAL NOTES

   NOTE 1: This driver is written to work with jumpers on the ILI9341 set to the
   following: IM0=0, IM1=1, IM2=1, IM3=1.  This jumper setting configures the
   ILI9341 to operates as a 4-wire 8-bit data serial interface II see page 26
   and 38 of datasheet for more details.

   NOTE 2: This driver implements both reading and writing to display.  Read is used
   for reading parameters from certain registers.  The maximum spi clock speed is
   16MHz but data sheet recommends 10MHz.

   NOTE 3: Most applications will not need the ability to read regsiters and are
   only writing to the display.  In this case MISO can be set to GPIO_NUM_NC.  Also
   the spi clock speed can be increased to either 26MHz or 40MHz.  The 2 displays I
   tested this driver with operated at 40MHz with no problem.
  ************************************************************************************/
#pragma once

#include <array>
#include <vector>
#include <smooth/core/io/Output.h>
#include <smooth/core/io/spi/Master.h>
#include <smooth/core/io/spi/SPIDevice.h>

namespace smooth::application::display
{
    class ILI9341 : public core::io::spi::SPIDevice
    {
        public:
            /// The constructor
            ILI9341(std::mutex& guard,
                    gpio_num_t chip_select_pin,
                    gpio_num_t data_command_pin,
                    gpio_num_t reset_pin,
                    gpio_num_t back_light_pin,
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

            /// Set backlight
            /// Turn backlight either ON or OFF
            void set_back_light(bool on)
            {
                on ? backlight_pin.set() : backlight_pin.clr();
            }

            /// Initialize the display
            /// \param host The SPI host either VSPI or HSPI
            /// \return true on success, false on failure
            bool init(spi_host_device_t host);

            /// Set the rotation of display
            /// Set the rotation of display using the MADCTL value see page 127 of datasheet
            /// \param madctl_value The MADCTL value
            /// \return true on success, false on failure
            bool set_rotation(uint8_t madctl_value);

            /// Reset the display
            void reset_display();

            /// Send init commands
            /// Send a sequence of commands and data to the display to perform intialization
            /// \return true on success, false on failure
            bool send_init_cmds();

            /// LCD write command
            /// \param cmd The command (register address) to write
            /// \return true on success, false on failure
            bool lcd_wr_cmd(const uint8_t cmd);

            /// LCD write data
            /// \param data The pointer to the first byte in the data
            /// \param length The number of bytes in the data
            /// \return true on success, false on failure
            bool lcd_wr_data(const uint8_t* data, size_t length);

            /// Send lines
            /// Sends color data to display starting at position x1,y1 and ending at position x2,y2
            /// \param x1 The top left corner x position
            /// \param y1 The top left corner y position
            /// \param x2 The bottom right corner x position
            /// \param y2 The bottom right corner y position
            /// \param data The pointer to the first byte in the data (color data)
            /// \param length The number of bytes in the data
            /// \return true on success, false on failure
            bool send_lines(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t* data, size_t length);

            /// Wait for send lines to finish;
            /// Waits for all SPI transactions in send lines to complete
            /// \return true on success, false on failure
            bool wait_for_send_lines_to_finish();

            /// Read parameters
            /// \param cmd The command (address register)
            /// \param rxdata The address of the container to hold the data
            /// \param param_count The number of parameters to be read
            /// \return true on success, false on failure
            bool read_params(uint8_t cmd, std::vector<uint8_t>& rxdata, uint32_t param_count);

            /// Read
            /// \param cmd The command or register address to read
            /// \param rxdata The pointer to the first byte location of the receive data container
            /// \param length The number of bytes to raed
            /// \return true on success, false on failure
            bool read(uint8_t cmd, const uint8_t* rxdata, size_t length);

        private:
            /// Pre Transmission Action
            /// The operations that need to be performed before the spi transaction is started
            /// \param trans The current spi transaction
            void pre_transmission_action(spi_transaction_t* trans) override
            {
                dc_pin.set( dc_pretrans_pin_states.at(current_transaction));
                cs_pin.set( cs_pretrans_pin_states.at(current_transaction));
            }

            /// Post Transmission Action
            /// The operations that need to be performed after the spi transaction has ended
            /// \param trans The current spi transaction
            void post_transmission_action(spi_transaction_t* trans) override
            {
                cs_pin.set( cs_posttrans_pin_states.at(current_transaction));
                current_transaction++;
            }

            smooth::core::io::Output reset_pin;
            smooth::core::io::Output backlight_pin;
            smooth::core::io::Output dc_pin;
            smooth::core::io::Output cs_pin;

            std::array<bool, 6> dc_pretrans_pin_states;
            std::array<bool, 6> cs_pretrans_pin_states;
            std::array<bool, 6> cs_posttrans_pin_states;
            std::size_t current_transaction;
    };
}
