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
#include <thread>
#include "smooth/application/display/SSD1306I2C.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::application::display
{
    //static const char* TAG = "SSD1306I2C";
    static const bool PIN_HIGH = true;
    static const bool PIN_LOW = false;

    SSD1306I2C::SSD1306I2C(i2c_port_t port, uint8_t address, std::mutex& guard) : I2CMasterDevice(port, address, guard)
    {
    }

    // Add reset pin
    void SSD1306I2C::add_reset_pin(std::unique_ptr<DisplayPin> reset_pin)
    {
        reset_pin = std::move(reset_pin);
    }

    // Hardware reset
    void SSD1306I2C::hw_reset(bool active_low,
                              std::chrono::milliseconds active_time,
                              std::chrono::milliseconds delay_time)
    {
        if (reset_pin != nullptr)
        {
            if (active_low)
            {
                reset_pin->set_output_level(PIN_LOW); // force the display chip to reset
                std::this_thread::sleep_for(active_time);
                reset_pin->set_output_level(PIN_HIGH);
                std::this_thread::sleep_for(delay_time);
            }
            else
            {
                reset_pin->set_output_level(PIN_HIGH); // force the display chip to reset
                std::this_thread::sleep_for(active_time);
                reset_pin->set_output_level(PIN_HIGH);
                std::this_thread::sleep_for(delay_time);
            }
        }
    }

    // Enter sleep mode
    bool SSD1306I2C::enter_sleep_mode()
    {
        return send_cmd(SSD1306Cmd::DisplayOff);
    }

    // Exit sleep mode
    bool SSD1306I2C::exit_sleep_mode()
    {
        return send_cmd(SSD1306Cmd::DisplayOn);
    }

    // Set the display inverted - white will be black and black will be white
    bool SSD1306I2C::set_display_inverted()
    {
        return send_cmd(SSD1306Cmd::InvertedDisplay);
    }

    // Set the display normal
    bool SSD1306I2C::set_display_normal()
    {
        return send_cmd(SSD1306Cmd::NormalDisplay);
    }

    // Send initialize sequence of commands and data to display
    bool SSD1306I2C::init_display()
    {
        std::vector<uint8_t> init_commands(begin(ssd1306_init_cmds), end(ssd1306_init_cmds));

        return send_cmds(init_commands);
    }

    // Send muliple commands to the display.
    bool SSD1306I2C::send_cmds(std::vector<uint8_t>& cmds)
    {
        return write(address, cmds);
    }

    // Send a single command to the display.
    bool SSD1306I2C::send_cmd(const uint8_t cmd)
    {
        std::vector<uint8_t> data{ ControlByte::SingleCommand };
        data.push_back(cmd);

        return write(address, data);
    }

    // Send data to the display ram
    bool SSD1306I2C::send_data_stream(std::vector<uint8_t>& data)
    {
        return write(address, data);
    }

    // Create the command stream for page address mode
    void SSD1306I2C::create_page_addr_mode_command_stream(std::vector<uint8_t>& cmd_stream,
                                                          uint8_t start_page,
                                                          uint8_t start_col)
    {
        cmd_stream.push_back(ControlByte::CommandStream);               // 0x00
        cmd_stream.push_back(SSD1306Cmd::MemoryAddressMode);            // 0x20
        cmd_stream.push_back(AddressMode::Page);                        // 0x02
        cmd_stream.push_back(SSD1306Cmd::PageAddress0 | start_page);    // 0xB0 - 0xB7
        cmd_stream.push_back(start_col & 0x0F);                         // start column lower nibble address
        cmd_stream.push_back(((start_col >> 4) & 0x0F) | SSD1306Cmd::UpperColumnAddress);         // start column upper
                                                                                                  // nibble address
    }

    // Create the command stream for vertical address mode
    void SSD1306I2C::create_vert_addr_mode_command_stream(std::vector<uint8_t>& cmd_stream,
                                                          uint8_t page_start,
                                                          uint8_t page_end,
                                                          uint8_t col_start,
                                                          uint8_t col_end)
    {
        cmd_stream.push_back(ControlByte::CommandStream);               // 0x00
        cmd_stream.push_back(SSD1306Cmd::MemoryAddressMode);            // 0x20
        cmd_stream.push_back(AddressMode::Vertical);                    // 0x01
        cmd_stream.push_back(SSD1306Cmd::PageAddress);                  // 0x22
        cmd_stream.push_back(page_start);
        cmd_stream.push_back(page_end);
        cmd_stream.push_back(SSD1306Cmd::ColumnAddress);                // 0x21
        cmd_stream.push_back(col_start);
        cmd_stream.push_back(col_end);
    }

    // Create the command stream for horizontal address mode
    void SSD1306I2C::create_horz_addr_mode_command_stream(std::vector<uint8_t>& cmd_stream,
                                                          uint8_t page_start,
                                                          uint8_t page_end,
                                                          uint8_t col_start,
                                                          uint8_t col_end)
    {
        cmd_stream.push_back(ControlByte::CommandStream);               // 0x00
        cmd_stream.push_back(SSD1306Cmd::MemoryAddressMode);            // 0x20
        cmd_stream.push_back(AddressMode::Horizontal);                  // 0x00
        cmd_stream.push_back(SSD1306Cmd::PageAddress);                  // 0x22
        cmd_stream.push_back(page_start);
        cmd_stream.push_back(page_end);
        cmd_stream.push_back(SSD1306Cmd::ColumnAddress);                // 0x21
        cmd_stream.push_back(col_start);
        cmd_stream.push_back(col_end);
    }

    // Fill display with the supplied fill value, black = 0x00, white = oxFF
    bool SSD1306I2C::fill_display(uint8_t fill_value)
    {
        bool res = false;
        std::vector<uint8_t> cmd_stream;
        std::array<uint8_t, 128> fill_data{};

        fill_data.fill(fill_value);

        // create pixel data stream that contains black pixels
        std::vector<uint8_t> pixel_data = { ControlByte::DataStream };
        pixel_data.insert(pixel_data.end(), fill_data.data(), fill_data.data() + fill_data.size());

        // Send pixel data stream to the 8 pages of the display
        for (uint8_t page = 0; page < 8; page++)
        {
            create_page_addr_mode_command_stream(cmd_stream, page, 0);
            res = send_cmds(cmd_stream);
            res &= send_data_stream(pixel_data);
        }

        return res;
    }
}
