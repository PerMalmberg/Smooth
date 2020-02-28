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

#include <cstdint>

namespace smooth::application::display
{
    // Common Lcd Display Commands
    enum LcdCmd : uint8_t
    {
        // 0x00-0x0F
        NOP = 0x00,             // Non Operation Instruction
        SWRESET = 0x01,         // Software Reset
        RDDID = 0x04,           // Read Display ID
        RDDST = 0x09,           // Read Display Status
        RDMODE = 0x0A,          // Read Display Power Mode
        RDMADCTL = 0x0B,        // Read Display MADCTL
        RDPIXFMT = 0x0C,        // Read Display Pixel Format
        RDIMGFMT = 0x0D,        // Read Display Image Format
        RDDSM = 0x0E,           // Read Display Signal Mode
        RDSELFDIAG = 0x0F,      // Read Display Self-Diagnostic Result

        //0x10-0x1F
        SLPIN = 0x10,           // Enter Sleep Mode
        SLPOUT = 0x11,          // Sleep Out
        PTLON = 0x12,           // Partial Mode ON
        NORON = 0x13,           // Normal Display Mode ON

        //0x20-0x2F
        INVOFF = 0x20,          // Display Inversion Off
        INVON = 0x21,           // Display Inversion On
        GAMMASET = 0x26,        // Gamma Set
        DISPOFF = 0x28,         // Display Off
        DISPON = 0x29,          // Display On
        CASET = 0x2A,           // Column Address Set
        RASET = 0x2B,           // Row Address Set
        RAMWR = 0x2C,           // Memory Write
        RAMRD = 0x2E,           // Memory Read

        //0x30-0x3F
        PTLAR = 0x30,           // Partial Area
        MADCTL = 0x36,          // Memory Data Access Control
        PIXFMT = 0x3A,          // Interface Pixel Format

        //0xB0-0xBF
        FRMCTR1 = 0xB1,         // Frame Rate Control (In normal mode/ Full colors)
        FRMCTR2 = 0xB2,         // Frame Rate Control (In Idle Mode/8 colors)
        FRMCTR3 = 0xB3,         // Frame Rate control (In Partial Mode/Full Colors)
        INVCTR = 0xB4,          // Display Inversion Control
        DFUNCTR = 0xB6,         // Display Function Control
        ENTRYM = 0xB7,          // Entry Mode Set
        VCOMS = 0xBB,           // VCOMS Setting

        // 0xC0-0xCF
        PWCTR1 = 0xC0,          // Power Control 1
        PWCTR2 = 0xC1,          // Power Control 2
        PWCTR3 = 0xC2,          // Power Control 3 (in Normal mode/ Full colors)
        PWCTR4 = 0xC3,          // Power Control 4 (in Idle mode/ 8-colors)
        PWCTR5 = 0xC4,          // Power Control 5 (in Partial mode/ full-colors)
        VMCTR1 = 0xC5,          // VCOM Control 1
        FRCTRL2 = 0xC6,         // Frame Rate Control in Normal Mode
        VMCTR2 = 0xC7,          // VCOM Offset Control
        POWERA = 0xCB,          // Power control A
        POWERB = 0xCF,          // Power control B

        // 0xD0-0xDF
        ST_PWCTR1 = 0xD0,       // Power Control 1 - ST7789V
        RDID1 = 0xDA,           // Read ID1 Value
        RDID2 = 0xDB,           // Read ID2 Value
        RDID3 = 0xDC,           // Read ID3 Value
        RDID4 = 0xDD,           // Read ID4 Value

        //0xE0-0xEF
        GMCTRP1 = 0xE0,         // Positive Gamma Correction
        GMCTRN1 = 0xE1,         // Negative Gamma Correction
        DTCA = 0xE8,            // Driver timing control A
        DTCB = 0xEA,            // Driver timing control B
        POWER_SEQ = 0xED,       // Power on sequence control

        //0xF0-0xFF
        THREEGAMMA_EN = 0xF2,   // Enable 3Gamma
        PRC = 0xF7,             // Pump ratio control
        PWCTR6 = 0xFC,          // Power Control 5 (in Partial mode + Idle mode)
    };
}
