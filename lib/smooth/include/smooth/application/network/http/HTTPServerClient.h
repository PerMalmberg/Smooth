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

#include <iostream>
#include <fstream>
#include <deque>
#include <smooth/core/network/ServerClient.h>
#include <smooth/application/network/http/HTTPProtocol.h>
#include <smooth/application/network/http/responses/StringResponse.h>
#include "IRequestHandler.h"
#include "URLEncoding.h"
#include "MIMEParser.h"

#include "IServerResponse.h"

namespace smooth::application::network::http
{
    static const std::chrono::milliseconds DefaultKeepAlive{5000};

    // TLS handshake takes a long time
    static const std::chrono::seconds SendTimeout{5};

    class HTTPServerClient
            : public smooth::core::network::ServerClient<HTTPServerClient, HTTPProtocol>,
              public IServerResponse
    {
        public:
            HTTPServerClient(smooth::core::Task& task,
                             smooth::core::network::ClientPool<HTTPServerClient>& pool,
                             std::size_t max_header_size,
                             std::size_t content_chunk_size)
                    : core::network::ServerClient<HTTPServerClient,
                    smooth::application::network::http::HTTPProtocol>(
                    task,
                    pool,
                    std::make_unique<smooth::application::network::http::HTTPProtocol>(max_header_size,
                                                                                       content_chunk_size,
                                                                                       *this)),
                      content_chunk_size(content_chunk_size)
            {
            }

            void
            event(const smooth::core::network::event::DataAvailableEvent<HTTPProtocol>& event) override;

            void event(const smooth::core::network::event::TransmitBufferEmptyEvent& /*event*/) override;

            void disconnected() override;

            void connected() override;

            void reset_client() override;

            std::chrono::milliseconds get_send_timeout() override
            {
                return SendTimeout;
            }

            void reply(std::unique_ptr<responses::IRequestResponseOperation> response) override;
            void reply_error(std::unique_ptr<responses::IRequestResponseOperation> response) override;

        private:
            bool parse_url(std::string& raw_url);

            void separate_request_parameters(std::string& url);

            void send_first_part();

            bool translate_method(const HTTPPacket& packet, HTTPMethod& method);

            const std::size_t content_chunk_size;
            std::unordered_map<std::string, std::string> request_parameters{};
            std::unordered_map<std::string, std::string> request_headers{};
            std::string requested_url{};
            URLEncoding encoding{};
            std::deque<std::unique_ptr<responses::IRequestResponseOperation>> operations{};
            std::unique_ptr<responses::IRequestResponseOperation> current_operation{};
            MIMEParser mime{};

            void set_keep_alive();
    };
}