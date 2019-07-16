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
        return regular ? regular->get_wanted_amount(packet) : websocket->get_wanted_amount(packet);
    }


    void HTTPProtocol::data_received(HTTPPacket& packet, int length)
    {
        if (regular)
        {
            regular->data_received(packet, length);
        }
        else
        {
            websocket->data_received(packet, length);
        }
    }

    uint8_t* HTTPProtocol::get_write_pos(HTTPPacket& packet)
    {
        return regular ? regular->get_write_pos(packet) : websocket->get_write_pos(packet);
    }


    bool HTTPProtocol::is_complete(HTTPPacket& packet) const
    {
        return regular ? regular->is_complete(packet) : websocket->is_complete(packet);
    }


    bool HTTPProtocol::is_error()
    {
        return regular ? regular->is_error() : websocket->is_error();
    }


    void HTTPProtocol::packet_consumed()
    {
        if (regular)
        {
            regular->packet_consumed();
        }
        else
        {
            websocket->packet_consumed();
        }
    }


    void HTTPProtocol::reset()
    {
        if (websocket)
        {
            websocket->reset();
        }
        regular = std::make_unique<RegularHTTPProtocol>(max_header_size,
                                                        content_chunk_size,
                                                        response,
                                                        *this);
    }

    void HTTPProtocol::upgrade_to_websocket()
    {
        regular.reset();
        websocket = std::make_unique<websocket::WebsocketProtocol>(content_chunk_size, response);
    }

}