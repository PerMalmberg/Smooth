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
#include <cstdint>

namespace smooth::application::display
{
    typedef struct LcdInitCmdType
    {
        uint8_t cmd;
        uint8_t data[16];
        uint8_t databytes;     //Num of bytes in data; bit 7 = delay after set; 0xFF = end of cmds.
    } lcd_init_cmd_t;

    // Command sequence to initialize ili9341. A command sequence contains a 3 items;
    // (1) a command byte, (2) the data to be sent and (3) the number of bytes in the data.
    // Note 1: Memory Access Control:
    //       M5Stack uses 0x08=landscape 0x68=portrait;
    //       Adafruit 2.8/3.2 Display uses 0x28=landscape 0x48=portrait;
    //       If using LittlevGL make sure lv_conf.h is configured for correct display rotation
    // Note 2: Pixel Format Set: 0x55 = 16bit,  0x66 = 18bit; also change lv_conf.h
    static constexpr const std::array<lcd_init_cmd_t, 22> ili_init_cmds =
    { {
        { 0xEF, { 0x03, 0x80, 0X02 }, 3 },                  // command sequence number 0
        { 0xCF, { 0x00, 0xC1, 0X30 }, 3 },                  // command sequence number 1
        { 0xED, { 0x64, 0x03, 0X12, 0X81 }, 4 },
        { 0xE8, { 0x85, 0x00, 0x78 }, 3 },
        { 0xCB, { 0x39, 0x2C, 0x00, 0x34, 0x02 }, 5 },
        { 0xF7, { 0x20 }, 1 },
        { 0xEA, { 0x00, 0x00 }, 2 },
        { 0xC0, { 0x23 }, 1 },          //Power control
        { 0xC1, { 0x10 }, 1 },          //Power control
        { 0xC5, { 0x3E, 0x28 }, 2 },    //VCOM control
        { 0xC7, { 0x86 }, 1 },          //VCOM control
        { 0x36, { 0x48 }, 1 },          //Memory Access Control  See Note 1 above  //0xA8=M5Stack
        { 0x3A, { 0x55 }, 1 },          //Pixel Format Set       See Note 2 above
        { 0xB1, { 0x00, 0x13 }, 2 },    // 0x18 79Hz, 0x1B default 70Hz, 0x13 100Hz
        { 0xB6, { 0x08, 0x82, 0x27 }, 3 },
        { 0xF2, { 0x00 }, 1 },
        { 0x26, { 0x01 }, 1 },
        { 0xE0, { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0X37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 }, 15 },
        { 0XE1, { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F }, 15 },
        { 0x11, { 0 }, 0x80 },      // Sleep Out cmd plus delay
        { 0x29, { 0 }, 0x80 },      // Display on cmd plus delay
        { 0, { 0 }, 0xff },         // End of sequence      // command sequence number 21
    } };

/*
        // another sequence of commands that also works.....
        static constexpr const std::array<lcd_init_cmd_t, 25> ili_init_cmds =
        {{
            { 0x28, {0x00}, 0 },                            // command sequence number 0
            { 0xCF, {0x00, 0x83, 0x30}, 3},
            { 0xED, {0x64, 0x03, 0x12, 0x81}, 4 },
            { 0xE8, {0x85, 0x01, 0x79}, 3 },
            { 0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
            { 0xF7, {0x20}, 1 },
            { 0xEA, {0x00, 0x00}, 2 },
            { 0xC0, {0x26}, 1 },		//Power control
            { 0xC1, {0x11}, 1 },		//Power control
            { 0xC5, {0x35, 0x3E}, 2 },	//VCOM control
            { 0xC7, {0xBE}, 1 },		//VCOM control
            { 0x36, {0x48}, 1 },		//Memory Access Control  See Note 1 above  //0x68
            { 0x3A, {0x55}, 1 },		//Pixel Format Set       See Note 2 above
            { 0xB1, {0x00, 0x1B}, 2 },
            { 0xF2, {0x08}, 1 },
            { 0x26, {0x01}, 1 },
            { 0xE0, {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0x87, 0X32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00}, 15 },
            { 0XE1, {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F}, 15 },
            { 0x2A, {0x00, 0x00, 0x00, 0xEF}, 4 },
            { 0x2B, {0x00, 0x00, 0x01, 0x3F}, 4 },
            { 0xB7, {0x07}, 1 },
            { 0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
            { 0x11, {0}, 0x80 },    // Sleep Out cmd plus delay
            { 0x29, {0}, 0x80 },    // Display on cmd plus delay
            { 0, {0}, 0xff },       // End of sequence      // command sequence number 24
        }};
*/
}
