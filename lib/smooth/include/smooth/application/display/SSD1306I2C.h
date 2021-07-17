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

#include <memory>
#include <array>
#include <vector>
#include "smooth/core/io/i2c/I2CMasterDevice.h"
#include "smooth/application/display/DisplayPin.h"

namespace smooth::application::display
{
    class SSD1306I2C : public core::io::i2c::I2CMasterDevice
    {
        public:
            // I2C Control Byte
            // b7 - Co - Continuation bit,
            //      if 1 no continuation only one byte to follow,
            //      if 0 the SDD1306 will expect mutiple bytes to follow
            // b6 - D/C - Data Command Select bit
            //      if 1 then next byte or a byte stream will be data
            //      if 0 then next byte or a byte stream will be commands
            enum ControlByte : uint8_t
            {
                SingleCommand = 0x80,
                CommandStream = 0x00,
                SingleData = 0xC0,
                DataStream = 0x40,
            };

            enum AddressMode : uint8_t
            {
                Horizontal = 0x00,
                Vertical = 0x01,
                Page = 0x02,
            };

            enum SSD1306Cmd : uint8_t
            {
                LowerColumnAddress = 0x00,
                UpperColumnAddress = 0x10,
                MemoryAddressMode = 0x20,
                ColumnAddress = 0x21,
                PageAddress = 0x22,
                DisplayStartLine = 0x40,
                ConstratControl = 0x81,
                SegmentReMapNormal = 0xA0,
                SegnentReMapReverse = 0xA1,
                DisplayOnWithRAM = 0XA4,
                DisplayOnIgnoreRAM = 0xA5,
                NormalDisplay = 0xA6,
                InvertedDisplay = 0xA7,
                MultiplexRatio = 0xA8,
                DisplayOff = 0xAE,
                DisplayOn = 0xAF,
                PageAddress0 = 0xB0,
                CommonOutputScanDirNormal = 0xC0,
                CommonOutputScanDirReversed = 0xC8,
                DisplayOffset = 0xD3,
                ClockDivideRatio = 0xD5,
                Precharge = 0xD9,
                ComPinsHwConfig = 0xDA,
                VcomDeselectLevel = 0xDB,
                Nop = 0xE3,
                HorizScrollSetup = 0x26,
                ContinuousVertHorizScrollSetup = 0x29,
                DeactivateScroll = 0x2E,
                ActivateScroll = 0x2F,
                SetVerticalScrollArea = 0xA3,
                ChargePump = 0x8D,
            };

            // init commands version 1
            static constexpr std::array<uint8_t, 24> ssd1306_init_cmds =
            {
                ControlByte::CommandStream,                 // 1:
                SSD1306Cmd::DisplayOff,                     // 2:     0xAE      - Turn Display off
                SSD1306Cmd::ClockDivideRatio, 0x80,         // 3,4:   0xD5:0x80 - Set display clock div ratio = 0x80
                SSD1306Cmd::MultiplexRatio, 0x3F,           // 5,6:   0xA8:0x3F - Set multiplex ratio for 128x64 (64-1)
                SSD1306Cmd::DisplayOffset, 0x00,            // 7,8:   0xD3:0x00 - Set display offset to 0x00
                SSD1306Cmd::DisplayStartLine,               // 9:     0x40      - Set display start line
                SSD1306Cmd::ChargePump, 0x14,               // 10,11: 0x8D:0x14 - Set charge pump 0x14=internal DC/DC
                SSD1306Cmd::SegnentReMapReverse,            // 12:    0xA1      - Set segment remap
                SSD1306Cmd::CommonOutputScanDirReversed,    // 13:    0xC8      - Set com output scan dir to reverse
                SSD1306Cmd::ComPinsHwConfig, 0x12,          // 14,15: 0xDA:0x12 - Set Com HW configuration -- 0x12 if
                                                            //                  - display height > 32 else 0x02
                SSD1306Cmd::ConstratControl, 0x7F,          // 16,17: 0x81:0x7F - Set contrast
                SSD1306Cmd::Precharge, 0xF1,                // 18,19: 0xD9:0xF1 - Set precharge period to 0xF1 =
                                                            //                  - for internal
                SSD1306Cmd::VcomDeselectLevel, 0x30,        // 20,21: 0xDB:0x40 - Set vcom deselect level
                SSD1306Cmd::DisplayOnWithRAM,               // 22:    0xA4      - Set display using RAM content
                SSD1306Cmd::NormalDisplay,                  // 23:    0xA6      - Set normal display not inverted
                SSD1306Cmd::DisplayOn,                      // 24:    0xAF      - Set display on
            };

            /// The constructor
            SSD1306I2C(i2c_port_t port, uint8_t address, std::mutex& guard);

            /// Add reset pin
            /// \param reset_pin
            void add_reset_pin(std::unique_ptr<DisplayPin> reset_pin);

            /// Hardware Reset
            /// \param active_low If true the reset pin requires a LOW to force the chip to reset,
            /// if false the reset pin requires a HIGH to force the chip to reset.
            /// \param active_time The amount of time in milliseconds the pin will be in the active state
            /// \param delay_time The amount of time in milliseconds after exiting the active state,
            /// the pin is the non-active state (waiting for the chip to complete it's reset).
            void hw_reset(bool active_low, std::chrono::milliseconds active_time, std::chrono::milliseconds delay_time);

            /// Place display in sleep mode
            /// \return true on success, false on failure
            bool enter_sleep_mode();

            /// Exit SLEEP mode
            /// \return true on success, false on failure
            bool exit_sleep_mode();

            /// Invert display
            /// \return true on success, false on failure
            bool set_display_inverted();

            /// Set display to normal
            /// \return true on success, false on failure
            bool set_display_normal();

            /// Send the intialization command to display
            /// \return true on success, false on failure
            bool init_display();

            /// Send Commands - write multiple commands to display
            /// \param cmds A vector that holds the commands
            /// \return true on success, false on failure
            bool send_cmds(std::vector<uint8_t>& cmds);

            /// Send Command - write single command to display
            /// \param cmd The command
            /// \return true on success, false on failure
            bool send_cmd(const uint8_t cmd);

            /// Send Data Stream - write data to display RAM
            /// \param data A vector that contains pixel data to send to display
            /// \return true on success, false on failure
            bool send_data_stream(std::vector<uint8_t>& data);

            /// Create page address mode command stream
            /// \param cmd_stream A vector that will hold the commands
            /// \param start_page The start page for the page address mode
            /// \param start_col The start column for the page address mode
            void create_page_addr_mode_command_stream(std::vector<uint8_t>& cmd_stream,
                                                      uint8_t start_page,
                                                      uint8_t start_col);

            /// Create vertical address mode command stream
            /// \param cmd_stream A vector that will hold the commands
            /// \param page_start The start page for the vertical address mode
            /// \param page_end The end page for the vertical address mode
            /// \param col_start The start column for the vertical address mode
            /// \param col_end The end column for the vertical address mode
            void create_vert_addr_mode_command_stream(std::vector<uint8_t>& cmd_stream,
                                                      uint8_t page_start,
                                                      uint8_t page_end,
                                                      uint8_t col_start,
                                                      uint8_t col_end);

            /// Create horizontal address mode command stream
            /// \param cmd_stream A vector that will hold the commands
            /// \param page_start The start page for the vertical address mode
            /// \param page_end The end page for the vertical address mode
            /// \param col_start The start column for the vertical address mode
            /// \param col_end The end column for the vertical address mode
            void create_horz_addr_mode_command_stream(std::vector<uint8_t>& cmd_stream,
                                                      uint8_t page_start,
                                                      uint8_t page_end,
                                                      uint8_t col_start,
                                                      uint8_t col_end);

            /// Fill display with the supplied fill value.
            /// \param fill_value The fill value; black = 0x00, white = 0xFF
            /// \return true on success, false on failure
            bool fill_display(uint8_t fill_value);

        private:
            std::unique_ptr<DisplayPin> reset_pin{};
    };
}
