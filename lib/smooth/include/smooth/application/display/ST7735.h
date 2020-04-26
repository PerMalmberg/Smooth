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
#include "DisplayTypes.h"
#include "LCDSpiCommands.h"

namespace smooth::application::display
{
    /************************************************************************************
      SPECIAL NOTES                SPECIAL NOTES                          SPECIAL NOTES

      The following is a collection of initialization sequences for the ST7735.  Some
      displays will require a combination of these init sequences depending upon the tab
      color of the protective plastic screen cover that is shipped with a new display.

      Note 1.  The M5StickC uses the ST7735S which requires special int_cmds sequence.
      The following code is an example of how to initialize the ST7735S display.
            send_init_cmds(init_cmds_R_part1.data(), init_cmds_R_part1.size());
            send_init_cmds(init_cmds_R_grn_tab_part2.data(), init_cmds_R_grn_tab_part2.size());
            send_cmd(LcdCmd::INVON);  // display inversion on
            send_init_cmds(init_cmds_R_part3.data(), init_cmds_R_part3.size());
    ************************************************************************************/

    // init commands for black tab - 17 commands
    static constexpr std::array<DisplayInitCmd, 17> init_cmds_7735B =
    { {
        { LcdCmd::SLPOUT, { 150 }, 0x80 },            // 1: SLPOUT(0x11) - Out of sleep mode, with delay 150ms
        { LcdCmd::PIXFMT, { 05, 10 }, 0x81 },         // 2: PIXFMT(0x3A) - Set color mode: 16-bit color 5-6-5 color
                                                      // format, 10ms delay
        { LcdCmd::FRMCTR1, { 0x00, 0x06, 0x03, 10 }, 83 },  // 3: FRMCTR1(0xB1) - fastest refresh,
                                                            // 6 lines front porch, 6 lines back porch, 10ms delay
        { LcdCmd::MADCTL, { 0x08 }, 1 },              // 4: MADCTL(0x36) - portrait
        { LcdCmd::DFUNCTR, { 0x15, 0x02 }, 2 },       // 5: DISSET5(0xB6) - clk cycle nonoverlap, cycle gate
        { LcdCmd::INVCTR, { 0x00 }, 1 },              // 6: INVCTR(0xB4) - Line inversion
        { LcdCmd::PWCTR1, { 0x02, 0x70, 10 }, 82 },   // 7: PWCTR1(0xC0) - GVDD = 4.7V, 1.0uA, delay 10ms
        { LcdCmd::PWCTR2, { 0x05 }, 1 },              // 8: PWCTR2(0xC1) - VGH = 14.7V, VGL = -7.35V
        { LcdCmd::PWCTR3, { 0x01, 0x02 }, 2 },        // 9: PWCTR3(0xC2) - OpAmp current small, Boost freq
        { LcdCmd::VMCTR1, { 0x3C, 0x38, 10 }, 82 },   // 10: VMCTR1(0xC5) -  VCOMH = 4V, VCOML = -1.1V, delay 10ms
        { LcdCmd::PWCTR6, { 0x11, 0x15 }, 2 },        // 11: PWCTR6(0xFC) -
        { LcdCmd::GMCTRP1,
          { 0x09, 0x16, 0x09, 0x20, 0x21, 0x1B, 0x13, 0x19, 0x17, 0x15, 0x1E, 0x2B, 0x04, 0x05, 0x02, 0x0E },
          16 },                                       // 12: Positive Gamma Correction(0xE0)
        { LcdCmd::GMCTRN1,
          { 0x0B, 0x14, 0x08, 0x1E, 0x22, 0x1D, 0x18, 0x1E, 0x1B, 0x1A, 0x24, 0x2B, 0x06, 0x06, 0x02, 0x0F },
          16 },                                       // 13: Negative Gamma Correction(0xE1) - with 10ms delay
        { LcdCmd::CASET, { 0x00, 0x02, 0x00, 0x81 }, 4 }, // 14: CASET(0x2A) - Column Address - Ystart=2, Yend=129
        { LcdCmd::RASET, { 0x00, 0x01, 0x00, 0xA0 }, 4 }, // 15: RASET(0x2B) - Row Address - Xstart=1, Xend=160
        { LcdCmd::NORON, { 10 }, 0x80 },              // 16: NORON(0x13) - Normal Display On - delay 10ms
        { LcdCmd::DISPON, { 255 }, 0x80 }             // 17: DISPON(0x29) - Main Screen Turn ON - delay 500ms
    } };

    // init commands for 7735R red or green tab, part 1 - 14 commands
    static constexpr std::array<DisplayInitCmd, 14> init_cmds_R_part1 =
    { {
        { LcdCmd::SLPOUT, { 150 }, 0x80 },            // 1: SLPOUT(0x11) - Out of sleep mode, with delay 150ms
        { LcdCmd::FRMCTR1, { 0x01, 0x2C, 0x2D }, 3 }, // 2: FRMCTR1(0xB1) - Normal Mode, Rate=fosc/(1X2+40)*(Line+2C+2D)
        { LcdCmd::FRMCTR2, { 0x01, 0x2C, 0x2D }, 3 }, // 3: FRMCTR2(0xB2) - Idle Mode, Rate=fosc/(1X2+40)*(Line+2C+2D)
        { LcdCmd::FRMCTR3, { 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D }, 6 }, // 4: FFRMCTR3(0xB3) - Partial Mode,
                                                                        // Dot inv and Line inv
        { LcdCmd::INVCTR, { 0x07 }, 1 },              // 5: INVCTR(0xB4) - No inversion
        { LcdCmd::PWCTR1, { 0xA2, 0x02, 0x84 }, 3 },  // 6: PWCTR1(0xC0) - -4.6V, AUTO mode
        { LcdCmd::PWCTR2, { 0xC5 }, 1 },              // 7: PWCTR2(0xC1) - VGH25=2.4V, VGSEL=-10, VGH=3*AVDD
        { LcdCmd::PWCTR3, { 0x0A, 0x00 }, 2 },        // 8: PWCTR3(0xC2) - OpAmp current small, Boost freq
        { LcdCmd::PWCTR4, { 0x8A, 0x2A }, 2 },        // 9: PWCTR4(0xC3) - BCLK/2, OpAmp current small, Medium low
        { LcdCmd::PWCTR5, { 0x8A, 0xEE }, 2 },        // 10: PWCTR5(0xC4) -
        { LcdCmd::VMCTR1, { 0x0E }, 1 },              // 11: VMCTR1(0xC5) - VCOM=-0.775v
        { LcdCmd::INVOFF, { 0x00 }, 0 },              // 12: INVOFF(0x20) - Don't invert display
        { LcdCmd::MADCTL, { 0xC8 }, 1 },              // 13: MADCTL(0x36) - 0xC8 = Landscape, 0x08 = portrait
        { LcdCmd::PIXFMT, { 0x05, 10 }, 0x81 },       // 14: COLMOD(0x3A) - Color Mode - 16-bit color, delay 10ms
    } };

    // init commands for 7735R green tab only, part 2 - 2 commands
    static constexpr std::array<DisplayInitCmd, 2> init_cmds_R_grn_tab_part2 =
    { {
        { LcdCmd::CASET, { 0x00, 0x02, 0x00, 0x81 }, 4 }, // 1: CASET(0x2A) - Column Address - Ystart=2, Yend=129
        { LcdCmd::RASET, { 0x00, 0x01, 0x00, 0xA0 }, 4 }, // 2: RASET(0x2B) - Row Address - Xstart=1, Xend=160
    } };

    // init commands for 7735R red tab only, part 2 - 2 commands
    static constexpr std::array<DisplayInitCmd, 2> init_cmds_R_red_tab_part2 =
    { {
        { LcdCmd::CASET, { 0x00, 0x00, 0x00, 0x7F }, 4 }, // 1: CASET(0x2A) - Column Address - Ystart=0, Yend=127
        { LcdCmd::RASET, { 0x00, 0x00, 0x00, 0x9F }, 4 }, // 2: RASET(0x2B) - Row Address - Xstart=0, Xend=159
    } };

    // init commands for 7735R green tab (1.44), part 2 - 2 commands
    static constexpr std::array<DisplayInitCmd, 2> init_cmds_R_grn_tab_144_part2 =
    { {
        { LcdCmd::CASET, { 0x00, 0x00, 0x00, 0x7F }, 4 }, // 1: CASET(0x2A) - Column Address - Ystart=0, Yend=127
        { LcdCmd::RASET, { 0x00, 0x00, 0x00, 0x7F }, 4 }, // 2: RASET(0x2B) - Row Address - Xstart=0, Xend=127
    } };

    // init commands for 7735R green tab (mini 160x80), part 2 - 2 commands
    static constexpr std::array<DisplayInitCmd, 2> init_cmds_R_grn_tab_mini_160x80_part2 =
    { {
        { LcdCmd::CASET, { 0x00, 0x00, 0x00, 0x4F }, 4 }, // 1: CASET(0x2A) - Column Address - Ystart=0, Yend=79
        { LcdCmd::RASET, { 0x00, 0x00, 0x00, 0x9F }, 4 }, // 2: RASET(0x2B) - Row Address - Xstart=0, Xend=159
    } };

    // init commands for 7735R red or green tab, part 3 - 4 commands
    static constexpr std::array<DisplayInitCmd, 4> init_cmds_R_part3 =
    { {
        { LcdCmd::GMCTRP1,
          { 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10 },
          16 },  // 1: Positive Gamma Correction(0xE0)
        { LcdCmd::GMCTRN1,
          { 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10 },
          16 },  // 2: Negative Gamma Correction(0xE1)
        { LcdCmd::NORON, { 10 }, 0x80 },               // 3: NORON(0x13) - Normal Display On - delay 10ms
        { LcdCmd::DISPON, { 150 }, 0x80 }              // 4: DISPON(0x29) - Main Screen Turn ON - delay 150ms
    } };

    /************************************************************************************
      SPECIAL NOTES                SPECIAL NOTES                          SPECIAL NOTES

      Some displays such as the ST7735x, offset the start row and start column from the
      normal positions of 0.  Different manufacturers may wire the TFT to map screen
      pixels to different areas of the CGRAM   The display driver for the ST7735 will
      usually require row-offsets and col-offsets to make the display show properly on
      the screen.  If the display is cut off or has extra 'random' pixels on the top
      and left, try using one of the following offsets.
    ************************************************************************************/

    static const CGRamOffset offsets_green_tab_160x80 = { 26, 1 };
    static const CGRamOffset offsets_green_tab_mini_160x80 = { 24, 0 };
    static const CGRamOffset offsets_green_tab_128 = { 0, 32 };
    static const CGRamOffset offsets_green_tab_1 = { 2, 1 };
    static const CGRamOffset offsets_green_tab_2 = { 2, 3 };
    static const CGRamOffset offsets_red_tab_160x80 = { 2, 3 };
}
