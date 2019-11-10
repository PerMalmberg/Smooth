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

#include "smooth/application/network/http/HTTPProtocol.h"
#include "smooth/application/network/http/regular//responses/ErrorResponse.h"
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
