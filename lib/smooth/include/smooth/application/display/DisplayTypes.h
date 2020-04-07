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

namespace smooth::application::display
{
    // display init command type structure
    struct DisplayInitCmd
    {
        uint8_t cmd;
        uint8_t data[16];
        uint8_t length;     //Num of bytes in data; bit 7 = delay when set;
    };

    // Some display such as the ST7735x require offset for the start row and start column from the
    // normal positions of 0.  Different manufacturers may wire the TFT to map screen pixels to
    // different areas of the CGRAM and therefore there is a need to account for this with the
    // struct CGRamOffset
    struct CGRamOffset
    {
        uint16_t col_offset;
        uint16_t row_offset;
    };
}
