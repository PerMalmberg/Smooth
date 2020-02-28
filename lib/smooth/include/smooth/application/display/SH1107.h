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
#include <array>

namespace smooth::application::display
{
    enum SH1107Cmd : uint8_t
    {
        LowerColumnAddress = 0x00,
        UpperColumnAddress = 0x10,
        PageAddressMode = 0x20,
        VerticalAddressMode = 0x21,
        ContrastControl = 0x81,
        SegmentReMapNormal = 0xA0,
        SegnentReMapReverse = 0xA1,
        MultiplexRation = 0xA8,
        NormalDisplayStatus = 0XA4,
        EntireDisplayStatus = 0xA5,
        NormalDisplay = 0xA6,
        ReverseDisplay = 0xA5,
        DisplayOffset = 0xD3,
        DcDcControlMode = 0xAD,
        DcDcModeOff = 0x80,
        DcDcModeOn = 0x81,
        DisplayOff = 0xAE,
        DisplayOn = 0xAF,
        PageAddress0 = 0xB0,
        CommonOutputScanDirPortrait = 0xC0,
        CommonOutputScanDirLandscape = 0xC8,
        ClockDivideRatio = 0xD5,
        DischargePrecharge = 0xD9,
        VcomDeselectLevel = 0xDB,
        DisplayStartLine = 0xDC,
        ReadModifyWrite = 0xE0,
        End = 0xEE,
        Nop = 0xE3
    };

    // init commands version 1
    static const std::array<uint8_t, 22> sh1107_init_cmds_1 =
    {
        SH1107Cmd::DisplayOff,                    // 1: 0xAE turn display off
        SH1107Cmd::DisplayStartLine, 0x00,        // 2,3: 0xDC set display start line to zero
        SH1107Cmd::ContrastControl, 0x2F,         // 4,5: 0x81 set display contrast
        SH1107Cmd::PageAddressMode,               // 6: 0x20 set memory addressing mode to page addressing
        SH1107Cmd::SegmentReMapNormal,            // 7: 0xA0 set segment re-map to non-rotated display
        SH1107Cmd::CommonOutputScanDirPortrait,   // 8: 0xC0 set com output scan direction to portrait
        SH1107Cmd::MultiplexRation, 0x7F,         // 9,10: 0xA8 set multiplex ratio to 128
        SH1107Cmd::DisplayOffset, 0x60,           // 11,12: 0xD3 set display offset to display start line COM96
        SH1107Cmd::ClockDivideRatio, 0x51,        // 13,14: 0xD5 set div ratio=1, freq=fosc
        SH1107Cmd::DischargePrecharge, 0x22,      // 15,16: 0xD9 set pre-charge= 2DCLKs, dis-charge=2DCLKs
        SH1107Cmd::VcomDeselectLevel, 0x35,       // 17,18: 0xDB set VCOM deselect level = 0.770
        SH1107Cmd::PageAddress0,                  // 19: 0xB0 set page address to load display RAM data
        SH1107Cmd::NormalDisplay,                 // 20: 0xA4 set entire display off
        SH1107Cmd::NormalDisplay,                 // 21: 0xA6 non-inverted display (normal display)
        SH1107Cmd::DisplayOn,                     // 22: 0XAF turn display on
    };
}
