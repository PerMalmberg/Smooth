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

   NOTE 1: The LCDSpi is written to work with jumpers on the ILI9341 set to
   the following: IM0=0, IM1=1, IM2=1, IM3=1.  This jumper setting configures
   the ILI9341 to operates as a 4-wire 8-bit data serial interface II see
   page 26 and 38 of datasheet for more details.

   NOTE 2: The maximum spi clock speed for reading parameters is 16MHz but data
   sheet recommends 10MHz.

   NOTE 3: Most applications will not need the ability to read regsiters and are
   only writing to the display.  In this case MISO can be set to GPIO_NUM_NC.  Also
   the spi clock speed can be increased to either 26MHz or 40MHz.  The 2 displays I
   tested with LCDSpi operated at 40MHz with no problem.
  ************************************************************************************/
#pragma once

#include <array>
#include "DisplayTypes.h"
#include "LCDSpiCommands.h"

namespace smooth::application::display
{
    // init commands version 1 - 20 commands
    static constexpr std::array<DisplayInitCmd, 20> ili9341_init_cmds_1 =
    { {
        { LcdCmd::POWERB, { 0x00, 0xC1, 0X30 }, 3 },              // 1: 0xCF - POWERB
        { LcdCmd::POWER_SEQ, { 0x64, 0x03, 0X12, 0X81 }, 4 },     // 2: 0xED - POWER_SEQ
        { LcdCmd::DTCA, { 0x85, 0x00, 0x78 }, 3 },                // 3: 0xE8 - DTCA
        { LcdCmd::POWERA, { 0x39, 0x2C, 0x00, 0x34, 0x02 }, 5 },  // 4: 0xCB - POWERA
        { LcdCmd::PRC, { 0x20 }, 1 },                             // 5: 0xF7 - PRC
        { LcdCmd::DTCB, { 0x00, 0x00 }, 2 },                      // 6: 0xEA - DTCB
        { LcdCmd::PWCTR1, { 0x23 }, 1 },                          // 7: 0xC0 - PWCTR1
        { LcdCmd::PWCTR2, { 0x10 }, 1 },                          // 8: 0xC1 - PWCTR2
        { LcdCmd::VMCTR1, { 0x3E, 0x28 }, 2 },                    // 9: 0xC5 - VMCTR1
        { LcdCmd::VMCTR2, { 0x86 }, 1 },                          // 10: 0xC7 - VMCTR2
        { LcdCmd::MADCTL, { 0x48 }, 1 },                          // 11: 0x36 - MADCTL  - portrait
        { LcdCmd::PIXFMT, { 0x55 }, 1 },                          // 12: 0x3A - PIXFMT  - 16 bit color
        { LcdCmd::FRMCTR1, { 0x00, 0x13 }, 2 },                   // 13: 0xB1 - FRMCTR1
        { LcdCmd::DFUNCTR, { 0x08, 0x82, 0x27 }, 3 },             // 14: 0xB6 -  DFUNCTR
        { LcdCmd::THREEGAMMA_EN, { 0x00 }, 1 },                   // 15: 0xF2 - 3GAMMA_EN
        { LcdCmd::GAMMASET, { 0x01 }, 1 },                        // 16: 0x26 - GAMMASET
        { LcdCmd::GMCTRP1, { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0X37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 },
          15 },                                                   // 17: 0xE0 - GMCTRP1
        { LcdCmd::GMCTRN1, { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F },
          15 },                                                   // 18: 0xE1 - GMCTRN1
        { LcdCmd::SLPOUT, { 0 }, 0x80 },                          // 19: 0x11 - SLPOUT
        { LcdCmd::DISPON, { 0 }, 0x80 },                          // 20: 0x29 - DISPON
    } };
}
