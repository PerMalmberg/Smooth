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

#include <smooth/application/network/http/HTTPProtocol.h>
#include <smooth/application/network/http/regular//responses/ErrorResponse.h>
#include <algorithm>

namespace smooth::application::network::http
{
    int HTTPProtocol::get_wanted_amount(HTTPPacket& packet)
    {
        int res;

        if (mode == Mode::Regular)
        {
            res = regular->get_wanted_amount(packet);
        }
        else
        {
            res = 0;
        }

        return res;
    }


    void HTTPProtocol::data_received(HTTPPacket& packet, int length)
    {
        if (mode == Mode::Regular)
        {
            regular->data_received(packet, length);
        }
        else
        {

        }
    }

    uint8_t* HTTPProtocol::get_write_pos(HTTPPacket& packet)
    {
        uint8_t* pos;

        if (mode == Mode::Regular)
        {
            pos = regular->get_write_pos(packet);
        }
        else
        {
            pos = nullptr;
        }

        return pos;
    }


    bool HTTPProtocol::is_complete(HTTPPacket& packet) const
    {
        bool res;

        if (mode == Mode::Regular)
        {
            res = regular->is_complete(packet);
        }
        else
        {
            res = 0;
        }

        return res;
    }


    bool HTTPProtocol::is_error()
    {
        bool res;

        if (mode == Mode::Regular)
        {
            res = regular->is_error();
        }
        else
        {
            res = false;
        }

        return res;
    }


    void HTTPProtocol::packet_consumed()
    {
        if (mode == Mode::Regular)
        {
            regular->packet_consumed();
        }
        else
        {

        }
    }


    void HTTPProtocol::reset()
    {
        if (mode == Mode::Regular)
        {
            regular->reset();
        }
        else
        {

        }
    }

}