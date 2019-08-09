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

#pragma once

#include <smooth/application/network/http/IResponseOperation.h>
#include <smooth/application/network/http/websocket/OpCode.h>

namespace smooth::application::network::http::websocket::responses
{
    class WSResponse
            : public IResponseOperation
    {
        public:
            explicit WSResponse(smooth::application::network::http::websocket::OpCode code);

            explicit WSResponse(const std::string& text, bool first_fragment, bool last_fragment);

            explicit WSResponse(std::string&& text, bool first_fragment, bool last_fragment);

            explicit WSResponse(const std::vector<uint8_t>& binary, bool treat_as_text, bool first_fragment,
                                bool last_fragment);

            explicit WSResponse(std::vector<uint8_t>&& binary, bool first_fragment, bool last_fragment);

            ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;

        private:
            bool header_sent{false};

            void set_length(uint64_t len, std::vector<uint8_t>& buff) const;

            smooth::application::network::http::websocket::OpCode op_code;

            bool first_fragment{true};
            bool last_fragment{true};

            std::vector<uint8_t> data{};
    };
}