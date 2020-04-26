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
#include <cstring>
#include <thread>
#include "smooth/application/display/LCDSpi.h"
#include "smooth/application/display/LCDSpiCommands.h"
#include "smooth/core/io/spi/SpiDmaFixedBuffer.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;
using namespace smooth::core::io::spi;

namespace smooth::application::display
{
    static const char* TAG = "LCDSpi";
    static const bool PIN_HIGH = true;
    static const bool PIN_LOW = false;

    LCDSpi::LCDSpi(std::mutex& guard,
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
                   bool use_post_transaction_callback)
            : SPIDevice(guard,
                        spi_command_bits,
                        spi_address_bits,
                        bits_between_address_and_data_phase,
                        spi_mode,
                        spi_positive_duty_cycle,
                        spi_cs_ena_posttrans,
                        spi_clock_speed_hz,
                        spi_device_flags,
                        spi_queue_size,
                        use_pre_transaction_callback,
                        use_post_transaction_callback),

              dc_pin(data_command_pin, true, false, false), // GPIO_MODE_OUTPUT, no pullup, no pulldown
              cs_pin(chip_select_pin, true, false, false)   // GPIO_MODE_OUTPUT, no pullup, no pulldown
    {
    }

    // Initialize the display
    bool LCDSpi::init(spi_host_device_t host)
    {
        // preset chip select pin high
        cs_pin.set(PIN_HIGH);

        // spi_transaction will not control chip select
        return initialize(host, GPIO_NUM_NC);
    }

    // Most display ICs use MADCTL to set screen orientation. Typically there
    // are four different screen orientations - Portrait, Landsacpe,
    // Portrait-Flipped and Landscape-Flipped.  The problem is there is not
    // a universal standard for setting the MADCTL value for a particular
    // screen oreintation.  For example M5Stack uses a MADCT value of 0x68
    // for portrait while the Adafruit 2.88 dipslay uses a MADCTL value of 0x48.
    // To make driver more versatile the function set_madctl is used to allow user
    // to set the screen orientation instead of the typical 0-3
    // screen_rotation_constant that is used in switch statement.
    // If you are using LittlevGL then you need to set the appropriate screen
    // width and screen height for the rotation you have chosen in the lvconf.h file.
    bool LCDSpi::set_madctl(uint8_t value, uint8_t madctl_reg)
    {
        return send_cmd(madctl_reg) & send_data(&value, 1);
    }

    // Hardware reset
    void LCDSpi::hw_reset(bool active_low,
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

    // Software reset
    bool LCDSpi::sw_reset(std::chrono::milliseconds delay_time)
    {
        bool res = send_cmd(static_cast<uint8_t>(LcdCmd::SWRESET));
        std::this_thread::sleep_for(delay_time);

        return res;
    }

    // Send initialize sequence of commands and data to display
    bool LCDSpi::send_init_cmds(const DisplayInitCmd* init_cmds, size_t length)
    {
        bool res = true;

        // check res everytime thru loop
        for (size_t index = 0; res && index < length; index++)
        {
            // send command
            res &= send_cmd(init_cmds[index].cmd);

            // send data
            uint8_t data_length = init_cmds[index].length;

            if (data_length >= 1 && data_length < 0x80)
            {
                res &= send_data(init_cmds[index].data, data_length);
            }

            // check to see if we have to delay before advancing to next sequence in ints_cmds
            if (data_length >= 0x80)
            {
                uint8_t delay_data_length = static_cast<uint8_t>(data_length & 0x7F);
                size_t delay_time = init_cmds[index].data[delay_data_length];

                // check delay_data_length to see if there is data to send before delaying
                if (delay_data_length > 0)
                {
                    res &= send_data(init_cmds[index].data, delay_data_length);
                }

                // delay value of 255 is really 500
                delay_time = delay_time == 255 ? 500 : delay_time;
                std::this_thread::sleep_for(std::chrono::milliseconds(delay_time));
            }
        }

        return res;
    }

    // Send muliple commands to the display.
    bool LCDSpi::send_cmds(const uint8_t* cmds, size_t length)
    {
        // reset current transaction counter
        current_transaction = 0;

        // setup pre and post control pin states
        prepare_pins_for_cmd_transaction(0);

        // create transaction
        std::lock_guard<std::mutex> lock(get_guard());
        spi_transaction_t trans{}; // zero out the transaction
        trans.length = length * 8;
        trans.tx_buffer = cmds;

        return polling_write(trans);
    }

    // Send a single command to the display.
    bool LCDSpi::send_cmd(const uint8_t cmd)
    {
        // reset current transaction counter
        current_transaction = 0;

        // setup pre and post control pin states
        prepare_pins_for_cmd_transaction(0);

        // create transaction
        std::lock_guard<std::mutex> lock(get_guard());
        spi_transaction_t trans{};  // zero out the transaction
        trans.length = 8;
        trans.tx_buffer = &cmd;

        return polling_write(trans);
    }

    // Send data to the display
    bool LCDSpi::send_data(const uint8_t* data, size_t length)
    {
        // reset current transaction counter
        current_transaction = 0;

        // setup pre and post control pin states
        prepare_pins_for_data_transaction(0);

        // create transaction
        std::lock_guard<std::mutex> lock(get_guard());
        spi_transaction_t trans{};  // zero out the transaction
        trans.length = length * 8;
        trans.tx_buffer = data;

        return polling_write(trans);
    }

    // Prepare pins for command transaction
    void LCDSpi::prepare_pins_for_cmd_transaction(size_t trans_num)
    {
        dc_pretrans_pin_states[trans_num] = PIN_LOW;
        cs_pretrans_pin_states[trans_num] = PIN_LOW;
        cs_posttrans_pin_states[trans_num] = PIN_HIGH;
    }

    // Prepare pins for data transaction
    void LCDSpi::prepare_pins_for_data_transaction(size_t trans_num)
    {
        dc_pretrans_pin_states[trans_num] = PIN_HIGH;
        cs_pretrans_pin_states[trans_num] = PIN_LOW;
        cs_posttrans_pin_states[trans_num] = PIN_HIGH;
    }

    // To send a set of lines we have to send a command, 2 data bytes, another command,
    // 2 more data bytes and another command before sending the lines of data itself;
    // a total of 6 transactions. We can't put all of this in just one transaction
    // because the D/C line needs to be toggled in the middle.
    // This routine queues these commands up as interrupt transactions so they get
    // sent faster (compared to calling spi_device_transmit several times)
    bool LCDSpi::send_lines(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint8_t* data, size_t length)
    {
        // if length is 0 nothing to do and probably an error so return
        bool res = length > 0;

        if (res)
        {
            std::lock_guard<std::mutex> lock(get_guard());

            // reset current transaction counter
            current_transaction = 0;

            // create transactions
            static std::array<spi_transaction_t, line_transaction_length> trans{};

            std::size_t x = 0;
            std::for_each(trans.begin(), trans.end(), [&](auto& current)
                          {
                              // zero out the transaction
                              std::memset(&current, 0, sizeof(current));

                              if ((x & 1) == 0)
                              {
                                  // even transfers are commands
                                  current.length = 8;
                                  prepare_pins_for_cmd_transaction(x);
                              }
                              else
                              {
                                  // odd transfers are data
                                  current.length = 8 * 4;
                                  prepare_pins_for_data_transaction(x);
                              }

                              current.flags = SPI_TRANS_USE_TXDATA;
                              x++;
                          });

            // column addresses
            trans[0].tx_data[0] = 0x2A;                                         // CASET
            trans[1].tx_data[0] = static_cast<uint8_t>((x1 >> 8) & 0xFF);       // Start Col High
            trans[1].tx_data[1] = static_cast<uint8_t>(x1 & 0xFF);              // Start Col Low
            trans[1].tx_data[2] = static_cast<uint8_t>((x2 >> 8) & 0xFF);       // End Col High
            trans[1].tx_data[3] = static_cast<uint8_t>(x2 & 0xFF);              // End Col Low

            // page addresses
            trans[2].tx_data[0] = 0x2B;                                         // PASET
            trans[3].tx_data[0] = static_cast<uint8_t>((y1 >> 8) & 0xFF);       // Start page high
            trans[3].tx_data[1] = static_cast<uint8_t>(y1 & 0xFF);              // start page low
            trans[3].tx_data[2] = static_cast<uint8_t>((y2 >> 8) & 0xFF);       // end page high
            trans[3].tx_data[3] = static_cast<uint8_t>(y2 & 0xFF);              // end page low

            // ram Write
            trans[4].tx_data[0] = 0x2C;                   // RAMWR - RAM write

            // data to send
            trans[5].tx_buffer = data;
            trans[5].length = length * 8;
            trans[5].flags = 0;                           // clear flags

            // queue all transactions.
            std::for_each(trans.begin(), trans.end(), [&](auto& t)
                          {
                              res &= queue_transaction(t);
                          });
        }

        return res;
    }

    // Wait for queued transactions to finish
    bool LCDSpi::wait_for_send_lines_to_finish()
    {
        bool res = true;

        std::lock_guard<std::mutex> lock(get_guard());

        for (uint8_t x = 0; x < line_transaction_length; x++)
        {
            res &= wait_for_transaction_to_finish();
        }

        return res;
    }

    // There are a few registers that can be read from the display chip and some
    // EXTENDED Command set of registers that some display chips may support.
    //
    // See pages 36-39 of the ili9341 data sheet that relates to the following discussion.
    // When reading more than 1 parameter, the ili9341 requires one dummy clock cycle before
    // reading the data.  For example Reg 0x04 Read Display ID info requires 25 clock cycles.
    // It is not easy to do this with the ESP32 SPI, so what we will do is add an extra byte
    // to read and produce 32 clock cycles.  The data read will have to be manipulated because
    // the MSB bit of the first parameter read contains the dummy clock data bit.  We need to
    // shift out the dummy data bit and recovery the MSB bit from the extra byte added to the read.
    // NOTE: FOR READING PARAMS SCK must be 16MHz or less the datasheet recommends 10MHz.
    bool LCDSpi::read_params(uint8_t cmd, std::vector<uint8_t>& data, uint32_t param_count)
    {
        SpiDmaFixedBuffer<uint8_t, 16> rxdata;

        bool res = false;

        if (param_count == 1)
        {
            res = read(cmd, rxdata.data(), 1);
            data.push_back(rxdata[0]);
        }
        else if (param_count > 1 && param_count < 16)
        {
            // increment param_count by 1 so an additional byte is read and we get all the data
            res = read(cmd, rxdata.data(), param_count + 1);

            // Since we are using 4-line spi (full duplex) the sck clocks both data out
            // and data in at the same time.  The first byte going out is the register we want
            // to read and so the first byte received is garbage or dummy byte.  So we
            // don't want to copy the dummy byte but copy only the real data
            for (size_t i = 0; i < param_count; i++)
            {
                data.push_back(static_cast<uint8_t>((rxdata[i] << 1) | (rxdata[i + 1] >> 7)));
            }
        }
        else
        {
            Log::error(TAG, "read_params() param_count must be greater than 0 or less than 16");
        }

        return res;
    }

    // Read a register or a sequence of registers from the display
    bool LCDSpi::read(uint8_t cmd, uint8_t* rxdata, size_t length)
    {
        // if length is 0 nothing to do and probably an error so return
        bool res = length > 0;

        if (res)
        {
            std::lock_guard<std::mutex> lock(get_guard());

            // create 2 spi transactions; one for command and one for reading parameters
            std::array<spi_transaction_t, 2> trans{}; // zero out the transaction

            // reset current transaction counter
            current_transaction = 0;

            // create command transaction
            auto& cmd_trans = trans[0];
            cmd_trans.length = 8;
            cmd_trans.tx_data[0] = cmd;
            cmd_trans.flags = SPI_TRANS_USE_TXDATA;

            // setup pre and post control pin states
            prepare_pins_for_cmd_transaction(0);

            // for reading parameter(s) we need to keep post trans cs pin low
            cs_posttrans_pin_states[0] = PIN_LOW;

            // create read parameters transaction
            auto& read_params = trans[1];
            read_params.rx_buffer = rxdata;
            read_params.rxlength = 8 * length;
            read_params.length = 8 * length;

            // setup pre and post control pin states
            prepare_pins_for_data_transaction(1);

            // queue the 2 transactions to be sent
            std::for_each(trans.begin(), trans.end(), [&](auto& t)
                          {
                              res &= queue_transaction(t);
                          });

            // wait for the 2 transaction to finish
            std::for_each(trans.begin(), trans.end(), [&](auto&)
                          {
                              res &= wait_for_transaction_to_finish();
                          });
        }

        return res;
    }

    // Add reset pin
    void LCDSpi::add_reset_pin(std::unique_ptr<DisplayPin> reset_pin)
    {
        reset_pin = std::move(reset_pin);
    }

    // Add backlight pin
    void LCDSpi::add_backlight_pin(std::unique_ptr<DisplayPin> bk_light_pin)
    {
        backlight_pin = std::move(bk_light_pin);
    }

    // Set backlight pin level
    void LCDSpi::set_back_light(bool level)
    {
        if (backlight_pin != nullptr)
        {
            backlight_pin->set_output_level(level);
        }
    }
}
