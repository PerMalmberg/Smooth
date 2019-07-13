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

#include <smooth/application/network/http/websocket/WebsocketProtocol.h>
#include <smooth/core/util/string_util.h>
#include <smooth/application/network/http/regular/HTTPHeaderDef.h>
#include <smooth/application/network/http/regular/RegularHTTPProtocol.h>
#include <smooth/application/network/http/regular/responses/ErrorResponse.h>

namespace smooth::application::network::http::websocket
{
    using namespace smooth::core;
    using namespace smooth::application::network::http::regular;

    int WebsocketProtocol::get_wanted_amount(HTTPPacket& /*packet*/)
    {
        return 0;
    }


    void WebsocketProtocol::data_received(HTTPPacket& /*packet*/, int /*length*/)
    {

    }

    uint8_t* WebsocketProtocol::get_write_pos(HTTPPacket& /*packet*/)
    {
        return nullptr;
    }


    bool WebsocketProtocol::is_complete(HTTPPacket& /*packet*/) const
    {
        return false;
    }


    bool WebsocketProtocol::is_error()
    {
        return false;
    }

    void WebsocketProtocol::packet_consumed()
    {

    }


    void WebsocketProtocol::reset()
    {

    }

}