// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <smooth/core/sntp/Sntp.h>

#include <lwip/apps/sntp.h>
#include <ctime>
#include <iostream>

namespace smooth::core::sntp
{

    Sntp::Sntp(const std::vector<std::string>& servers)
            : servers(servers)
    {
    }

    void Sntp::start()
    {
        if (!started)
        {
            sntp_setoperatingmode(SNTP_OPMODE_POLL);

            uint8_t i = 0;
            for (auto& s : servers)
            {
                // Only a pointer to the string is stored in the underlying structs.
                sntp_setservername(i++, const_cast<char*>(s.c_str()));
            }

            sntp_init();
        }
    }

    bool Sntp::is_time_set() const
    {
        time_t now{};
        tm timeinfo{};
        time(&now);
        localtime_r(&now, &timeinfo);

        return timeinfo.tm_year > 70;
    }
}

