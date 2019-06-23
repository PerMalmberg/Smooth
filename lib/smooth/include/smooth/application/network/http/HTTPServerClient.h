#pragma once

#include <iostream>
#include <fstream>
#include <deque>
#include <smooth/core/network/ServerClient.h>
#include <smooth/application/network/http/HTTPProtocol.h>
#include <smooth/application/network/http/responses/Response.h>
#include "IRequestHandler.h"
#include "URLEncoding.h"

#include "IResponseQueue.h"

namespace smooth::application::network::http
{
    static const std::chrono::milliseconds DefaultKeepAlive{5000};

    // TLS handshake takes a long time
    static const std::chrono::seconds SendTimeout{5};

    class HTTPServerClient
            : public smooth::core::network::ServerClient<HTTPServerClient, HTTPProtocol>,
              IResponseQueue
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
                                                                                       content_chunk_size)),
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

            void enqueue(std::unique_ptr<responses::IRequestResponseOperation> response) override;

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

            void set_keep_alive();
    };
}