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

#include <iostream>
#include <fstream>
#include <deque>
#include "smooth/core/network/ServerClient.h"
#include "smooth/application/network/http/HTTPProtocol.h"
#include "smooth/application/network/http/regular/responses/StringResponse.h"
#include "smooth/application/network/http/IConnectionTimeoutModifier.h"
#include "smooth/application/network/http/websocket/WebsocketServer.h"
#include "regular/IRequestHandler.h"
#include "URLEncoding.h"
#include "IServerResponse.h"
#include "IResponseOperation.h"

namespace smooth::application::network::http
{
    using namespace smooth::application::network::http::regular;

    static const std::chrono::milliseconds DefaultKeepAlive{ 5000 };

    // TLS handshake takes a long time
    static const std::chrono::seconds SendTimeout{ 5 };

    class HTTPServerClient
        : public smooth::core::network::ServerClient<HTTPServerClient, HTTPProtocol, IRequestHandler>,
        public IServerResponse,
        public IConnectionTimeoutModifier
    {
        public:
            HTTPServerClient(smooth::core::Task& task,
                             smooth::core::network::ClientPool<HTTPServerClient>& pool,
                             std::size_t max_header_size,
                             std::size_t content_chunk_size,
                             std::size_t max_enqueued_responses)
                    : core::network::ServerClient<HTTPServerClient,
                                                  smooth::application::network::http::HTTPProtocol, IRequestHandler>(
                          task,
                          pool,
                          std::make_unique<smooth::application::network::http::HTTPProtocol>(
                                                                                       max_header_size,
                                                                                       content_chunk_size,
                                                                                       *this)),
                      content_chunk_size(content_chunk_size),
                      task(task),
                      max_enqueued_responses(max_enqueued_responses)
            {
            }

            void
            event(const smooth::core::network::event::DataAvailableEvent<HTTPProtocol>& event) override;

            void http_event(const smooth::core::network::event::DataAvailableEvent<HTTPProtocol>& event);

            void websocket_event(const smooth::core::network::event::DataAvailableEvent<HTTPProtocol>& event);

            void event(const smooth::core::network::event::TransmitBufferEmptyEvent& /*event*/) override;

            void disconnected() override;

            void connected() override;

            void reset_client() override;

            std::chrono::milliseconds get_send_timeout() override
            {
                return SendTimeout;
            }

            void reply(std::unique_ptr<IResponseOperation> response, bool place_first) override;

            void reply_error(std::unique_ptr<IResponseOperation> response) override;

            void set_receive_timeout(const std::chrono::milliseconds& timeout) override
            {
                socket->set_receive_timeout(timeout);
            }

        protected:
            smooth::core::Task& get_task() override
            {
                return task;
            }

            void upgrade_to_websocket_internal() override
            {
                // Don't clear TX buffer - the upgrade response is being sent.
                container->get_rx_buffer().clear();
                container->get_protocol().upgrade_to_websocket();
                mode = Mode::Websocket;
            }

        private:
            enum class Mode
            {
                HTTP,
                Websocket
            };

            Mode mode{ Mode::HTTP };

            bool parse_url(std::string& raw_url);

            void separate_request_parameters(std::string& url);

            void send_first_part();

            bool translate_method(const HTTPPacket& packet, HTTPMethod& method) const;

            const std::size_t content_chunk_size;
            smooth::core::Task& task;
            std::unordered_map<std::string, std::string> request_parameters{};
            std::unordered_map<std::string, std::string> request_headers{};
            std::string requested_url{};
            URLEncoding encoding{};
            std::deque<std::unique_ptr<IResponseOperation>> operations{};
            std::unique_ptr<IResponseOperation> current_operation{};
            const std::size_t max_enqueued_responses;

            void set_keep_alive();
    };
}
