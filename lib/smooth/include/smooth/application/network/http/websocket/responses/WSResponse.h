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

#pragma once

#include "smooth/application/network/http/IResponseOperation.h"
#include "smooth/application/network/http/websocket/OpCode.h"

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
            bool header_sent{ false };

            void set_length(uint64_t len, std::vector<uint8_t>& buff) const;

            smooth::application::network::http::websocket::OpCode op_code;

            bool first_fragment{ true };
            bool last_fragment{ true };

            std::vector<uint8_t> data{};
    };
}
