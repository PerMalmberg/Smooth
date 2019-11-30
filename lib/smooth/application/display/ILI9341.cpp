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
#include "smooth/application/display/ILI9341.h"
#include "smooth/application/display/ILI9341_init_cmds.h"
#include "smooth/core/io/spi/SpiDmaFixedBuffer.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;
using namespace smooth::core::io::spi;

namespace smooth::application::display
{
    static const char* TAG = "ILI9314";
    static const bool PIN_HIGH = true;
    static const bool PIN_LOW = false;

    ILI9341::ILI9341(std::mutex& guard,
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

              reset_pin(reset_pin, true, false, false),
              backlight_pin(back_light_pin, true, false, false),
              dc_pin(data_command_pin, true, false, false),
              cs_pin(chip_select_pin, true, false, false)
    {
    }

    // Initialize the display
    bool ILI9341::init(spi_host_device_t host)
    {
        // spi_transaction will not control chip select
        bool res = initialize(host, GPIO_NUM_NC);

        return res;
    }

    // I have found different displays set the value of madctl differently
    // for the same rotation.  For example M5Stack sets madctl to 0x68 for
    // portrait while the adafruit 2.88 dipslay sets madctl to 0x48.  To make
    // driver more versatile I have used madctl_value instead of the typical
    // 0-3 screen_rotation_value that is used in switch statement.
    // If you are using LittlevGL then you need to set the appropriate screen
    // width and screen height for the rotation you have chosen in the lvconf.h file.
    bool ILI9341::set_rotation(uint8_t madctl_value)
    {
        bool res = lcd_wr_cmd(0x36);
        res &= lcd_wr_data(&madctl_value, 1);

        return res;
    }

    // Reset the display
    void ILI9341::reset_display()
    {
        reset_pin.set(PIN_LOW);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        reset_pin.set(PIN_HIGH);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Send initialize sequence of commands and data to display
    bool ILI9341::send_init_cmds()
    {
        bool res = true;

        uint16_t seq_num = 0;

        while (ili_init_cmds.at(seq_num).databytes != 0xff)
        {
            // check if delay is needed; send only command then delay
            if (ili_init_cmds.at(seq_num).databytes & 0x80)
            {
                res &= lcd_wr_cmd(ili_init_cmds.at(seq_num).cmd);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            else
            {
                res &= lcd_wr_cmd(ili_init_cmds.at(seq_num).cmd);
                res &= lcd_wr_data(ili_init_cmds.at(seq_num).data, ili_init_cmds.at(seq_num).databytes & 0x1F);
            }

            // advance to next sequence in ili_ints_cmds
            seq_num++;
        }

        return res;
    }

    // Send a command to the LCD. Uses spi_device_polling_write, which waits
    // until the transfer is complete.
    //
    // Since command transactions are usually small, they are handled in polling
    // mode for higher speed. The overhead of interrupt transactions is more than
    // just waiting for the transaction to complete.
    bool ILI9341::lcd_wr_cmd(const uint8_t cmd)
    {
        // reset current transaction counter
        current_transaction = 0;

        // setup pre and post control pin states
        dc_pretrans_pin_states.at(0) = PIN_LOW;
        cs_pretrans_pin_states.at(0) = PIN_LOW;
        cs_posttrans_pin_states.at(0) = PIN_HIGH;

        std::lock_guard<std::mutex> lock(get_guard());
        spi_transaction_t trans;
        std::memset(&trans, 0, sizeof(trans));  //Zero out the transaction
        trans.rx_buffer = NULL;
        trans.rxlength = 0;
        trans.length = 8;
        trans.tx_buffer = &cmd;

        bool res = polling_write(trans);

        return res;
    }

    // Send data to the LCD. Uses spi_device_polling_write, which waits until the
    // transfer is complete.
    bool ILI9341::lcd_wr_data(const uint8_t* data, size_t length)
    {
        // reset current transaction counter
        current_transaction = 0;

        // setup pre and post control pin states
        dc_pretrans_pin_states.at(0) = PIN_HIGH;
        cs_pretrans_pin_states.at(0) = PIN_LOW;
        cs_posttrans_pin_states.at(0) = PIN_HIGH;

        std::lock_guard<std::mutex> lock(get_guard());
        spi_transaction_t trans;
        std::memset(&trans, 0, sizeof(trans));  //Zero out the transaction
        trans.rx_buffer = NULL;
        trans.rxlength = 0;
        trans.length = length * 8;
        trans.tx_buffer = data;

        bool res = polling_write(trans);

        return res;
    }

    // To send a set of lines we have to send a command, 2 data bytes, another command,
    // 2 more data bytes and another command before sending the lines of data itself;
    // a total of 6 transactions. (We can't put all of this in just one transaction
    // because the D/C line needs to be toggled in the middle.)
    // This routine queues these commands up as interrupt transactions so they get
    // sent faster (compared to calling spi_device_transmit several times), and
    // meanwhile the lines for next transactions can get calculated.
    bool ILI9341::send_lines(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t* data, size_t length)
    {
        // if length is 0 nothing to do and probably an error so return
        bool res = length > 0;

        if (res)
        {
            std::lock_guard<std::mutex> lock(get_guard());

            // reset current transaction counter
            current_transaction = 0;

            // Transaction descriptors. Declared static so they're not allocated on the
            // stack; we need this memory even when this function is finished because
            // the SPI driver needs access to it even while we're already calculating
            // the next line.
            static std::array<spi_transaction_t, 6> trans;

            // In theory, it's better to initialize trans and data only once and hang
            // on to the initialized variables. We allocate them on the stack, so we
            // don't need to re-init them each call.
            for (uint8_t x = 0; x < 6; x++)
            {
                //Zero out the transaction
                std::memset(&trans.at(x), 0, sizeof(spi_transaction_t));

                if ((x & 1) == 0)
                {
                    // Even transfers are commands
                    trans.at(x).rx_buffer = NULL;
                    trans.at(x).rxlength = 0;
                    trans.at(x).length = 8;

                    // setup pre and post states
                    dc_pretrans_pin_states.at(x) = PIN_LOW;
                    cs_pretrans_pin_states.at(x) = PIN_LOW;
                    cs_posttrans_pin_states.at(x) = PIN_HIGH;
                }
                else
                {
                    // Odd transfers are data
                    trans.at(x).rx_buffer = NULL;
                    trans.at(x).rxlength = 0;
                    trans.at(x).length = 8 * 4;

                    // setup pre and post states
                    dc_pretrans_pin_states.at(x) = PIN_HIGH;
                    cs_pretrans_pin_states.at(x) = PIN_LOW;
                    cs_posttrans_pin_states.at(x) = PIN_HIGH;
                }

                trans.at(x).flags = SPI_TRANS_USE_TXDATA;
            }

            // Column addresses
            trans.at(0).tx_data[0] = 0x2A;
            trans.at(1).tx_data[0] = static_cast<uint8_t>((x1 >> 8) & 0xFF);       // Start Col High
            trans.at(1).tx_data[1] = static_cast<uint8_t>(x1 & 0xFF);              // Start Col Low
            trans.at(1).tx_data[2] = static_cast<uint8_t>((x2 >> 8) & 0xFF);       // End Col High
            trans.at(1).tx_data[3] = static_cast<uint8_t>(x2 & 0xFF);              // End Col Low

            // Page addresses
            trans.at(2).tx_data[0] = 0x2B;
            trans.at(3).tx_data[0] = static_cast<uint8_t>((y1 >> 8) & 0xFF);       // Start page high
            trans.at(3).tx_data[1] = static_cast<uint8_t>(y1 & 0xFF);              // start page low
            trans.at(3).tx_data[2] = static_cast<uint8_t>((y2 >> 8) & 0xFF);       // end page high
            trans.at(3).tx_data[3] = static_cast<uint8_t>(y2 & 0xFF);              // end page low

            // Ram Write
            trans.at(4).tx_data[0] = 0x2C;                   // memory write

            // Data to send
            trans.at(5).tx_buffer = data;
            trans.at(5).length = length * 8;
            trans.at(5).flags = 0;                           // clear flags

            // Queue all transactions.
            for (uint8_t i = 0; i < 6; i++)
            {
                res &= queue_transaction(trans.at(i));
            }
        }

        return res;
    }

    // Wait for queued transactions to finish
    bool ILI9341::wait_for_send_lines_to_finish()
    {
        bool res = true;

        std::lock_guard<std::mutex> lock(get_guard());

        for (uint8_t x = 0; x < 6; x++)
        {
            res &= wait_for_transaction_to_finish();
        }

        return res;
    }

    // There are a few registers that can be read from the ILI9341 and some
    // EXTENDED Command set of registers that some displays may support (page 85)
    //
    // See pages 36-39 of the ili9341 data sheet that relates to the following discussion.
    // When reading more than 1 parameter, the ILI9341 requires one dummy clock cycle before
    // reading the data.  For example Reg 0x04 Read Display ID info requires 25 clock cycles.
    // It is not easy to do this with the ESP32 SPI, so what we will do is add an extra byte
    // to read and produce 32 clock cycles.  The data read will have to be manipulated because
    // the MSB bit of the first parameter read contains the dummy clock data bit.  We need to
    // shift out the dummy data bit and recovery the MSB bit from the extra byte added to the read.
    // NOTE: FOR READING PARAMS SCK must be 16MHz or less the datasheet recommends 10MHz.
    bool ILI9341::read_params(uint8_t cmd, std::vector<uint8_t>& data, uint32_t param_count)
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
                data.push_back( static_cast<uint8_t>((rxdata[i] << 1) | (rxdata[i + 1] >> 7)));
            }
        }
        else
        {
            Log::error(TAG, "read_params() param_count must be greater than 0 or less than 16");
        }

        return res;
    }

    // Read a register or a sequence of registers from the display
    bool ILI9341::read(uint8_t cmd, uint8_t* rxdata, size_t length)
    {
        // if length is 0 nothing to do and probably an error so return
        bool res = length > 0;

        if (res)
        {
            std::lock_guard<std::mutex> lock(get_guard());

            // create 2 spi transactions; one for command and one for reading parameters
            std::array<spi_transaction_t, 2> trans;

            for (uint8_t x = 0; x < 2; x++)
            {
                //Zero out the transaction
                std::memset(&trans.at(x), 0, sizeof(spi_transaction_t));
            }

            // reset current transaction counter
            current_transaction = 0;

            // configure command transaction
            trans.at(0).rx_buffer = NULL;
            trans.at(0).rxlength = 0;
            trans.at(0).length = 8;
            trans.at(0).tx_data[0] = cmd;
            trans.at(0).flags = SPI_TRANS_USE_TXDATA;

            // setup pre and post control pin states
            dc_pretrans_pin_states.at(0) = PIN_LOW;
            cs_pretrans_pin_states.at(0) = PIN_LOW;
            cs_posttrans_pin_states.at(0) = PIN_LOW;  // keep chip select low

            // configure read parameters transaction
            trans.at(1).rx_buffer = rxdata;
            trans.at(1).rxlength = 8 * length;
            trans.at(1).length = 8 * length;
            trans.at(1).tx_buffer = NULL;

            // setup pre and post control pin states
            dc_pretrans_pin_states.at(1) = PIN_HIGH;
            cs_pretrans_pin_states.at(1) = PIN_LOW;
            cs_posttrans_pin_states.at(1) = PIN_HIGH;

            // Queue the 2 transactions to be sent
            for (uint8_t i = 0; i < 2; i++)
            {
                res &= queue_transaction(trans.at(i));
            }

            // wait for the 2 transaction to finish
            for (uint8_t x = 0; x < 2; x++)
            {
                res &= wait_for_transaction_to_finish();
            }
        }

        return res;
    }
}
