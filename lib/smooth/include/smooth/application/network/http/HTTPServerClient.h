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

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                template<int MaxHeaderSize, int ContentChuckSize>
                class HTTPServerClient
                        : public smooth::core::network::ServerClient<HTTPServerClient<MaxHeaderSize, ContentChuckSize>, HTTPProtocol<MaxHeaderSize, ContentChuckSize>>,
                          IResponseQueue
                {
                    public:
                        explicit HTTPServerClient(smooth::core::Task& task,
                                                  smooth::core::network::ClientPool<HTTPServerClient>& pool)
                                : core::network::ServerClient<
                                HTTPServerClient<MaxHeaderSize, ContentChuckSize>,
                                smooth::application::network::http::HTTPProtocol<MaxHeaderSize, ContentChuckSize>>(task,
                                                                                                                   pool)
                        {
                        }

                        void
                        event(const smooth::core::network::event::DataAvailableEvent<HTTPProtocol<MaxHeaderSize, ContentChuckSize>>& event) override;

                        void event(const smooth::core::network::event::TransmitBufferEmptyEvent& /*event*/) override;

                        void disconnected() override;

                        void connected() override;

                        void reset_client() override;

                        std::chrono::milliseconds get_send_timeout() override
                        {
                            return std::chrono::seconds{1};
                        }

                        void enqueue(std::unique_ptr<responses::IRequestResponseOperation> response) override;

                    private:
                        bool parse_url(std::string& raw_url);

                        void separate_request_parameters(std::string& url);

                        void send_first_part();

                        std::unordered_map<std::string, std::string> request_parameters{};
                        std::unordered_map<std::string, std::string> request_headers{};
                        std::string requested_url{};
                        URLEncoding encoding{};
                        std::deque<std::unique_ptr<responses::IRequestResponseOperation>> operations{};
                        std::unique_ptr<responses::IRequestResponseOperation> current_operation{};
                };

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::event(
                        const core::network::event::DataAvailableEvent<HTTPProtocol<MaxHeaderSize, ContentChuckSize>>& event)
                {
                    typename HTTPProtocol<MaxHeaderSize, ContentChuckSize>::packet_type packet;
                    if (event.get(packet))
                    {
                        bool first_packet = !packet.is_continuation();
                        bool res = true;

                        if (first_packet)
                        {
                            // First packet, parse URL etc.
                            request_headers.clear();
                            std::swap(request_headers, packet.headers());
                            requested_url = std::move(packet.get_request_url());
                            res = parse_url(requested_url);
                        }

                        if (res)
                        {
                            auto* context = reinterpret_cast<IRequestHandler<MaxHeaderSize, ContentChuckSize>*>(
                                    this->get_client_context());

                            if (context)
                            {

                                if (packet.get_request_method() == "POST")
                                {
                                    context->handle_post(*this,
                                                         requested_url,
                                                         request_headers,
                                                         request_parameters,
                                                         packet.get_buffer(),
                                                         first_packet,
                                                         !packet.is_continued());
                                }
                                else if (packet.get_request_method() == "GET")
                                {
                                    context->handle_post(*this,
                                                         requested_url,
                                                         request_headers,
                                                         request_parameters,
                                                         packet.get_buffer(),
                                                         first_packet,
                                                         !packet.is_continued());
                                }
                                else if (packet.get_request_method() == "DELETE")
                                {
                                    context->handle_delete(*this,
                                                         requested_url,
                                                         request_headers,
                                                         request_parameters,
                                                         packet.get_buffer(),
                                                         first_packet,
                                                         !packet.is_continued());
                                }
                                else if (packet.get_request_method() == "HEAD")
                                {
                                    context->handle_head(*this,
                                                         requested_url,
                                                         request_headers,
                                                         request_parameters,
                                                         packet.get_buffer(),
                                                         first_packet,
                                                         !packet.is_continued());
                                }
                                else if (packet.get_request_method() == "PUT")
                                {
                                    context->handle_put(*this,
                                                         requested_url,
                                                         request_headers,
                                                         request_parameters,
                                                         packet.get_buffer(),
                                                         first_packet,
                                                         !packet.is_continued());
                                }
                                else
                                {
                                    // Unsupported method.
                                    enqueue(std::make_unique<responses::Response>(ResponseCode::Method_Not_Allowed));
                                }
                            }
                        }
                    }
                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void
                HTTPServerClient<MaxHeaderSize, ContentChuckSize>::event(
                        const smooth::core::network::event::TransmitBufferEmptyEvent&)
                {
                    if (current_operation)
                    {
                        std::vector<uint8_t> data;
                        auto res = current_operation->get_data(ContentChuckSize, data);
                        HTTPPacket p{data};
                        if (res == responses::ResponseStatus::AllSent)
                        {
                            current_operation.reset();
                        }

                        auto& tx = this->get_buffers()->get_tx_buffer();
                        tx.put(p);
                    }
                    else
                    {
                        send_first_part();
                    }
                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::disconnected()
                {

                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::connected()
                {

                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::reset_client()
                {

                }

                template<int MaxHeaderSize, int ContentChuckSize>
                bool HTTPServerClient<MaxHeaderSize, ContentChuckSize>::parse_url(std::string& raw_url)
                {
                    separate_request_parameters(raw_url);

                    auto res = encoding.decode(raw_url);

                    return res;
                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::separate_request_parameters(std::string& url)
                {
                    // Only supporting key=value format.
                    request_parameters.clear();

                    auto pos = std::find(url.begin(), url.end(), '?');
                    if (pos != url.end())
                    {
                        encoding.decode(url, pos, url.end());

                        pos++;
                        while (pos != url.end())
                        {
                            auto equal_sign = std::find(pos, url.end(), '=');
                            if (equal_sign != url.end())
                            {
                                // Find ampersand or end of string
                                auto ampersand = std::find(equal_sign, url.end(), '&');
                                auto key = std::string(pos, equal_sign);
                                auto value = std::string{++equal_sign, ampersand};
                                request_parameters[key] = value;
                                if (ampersand != url.end())
                                {
                                    ++ampersand;
                                }
                                pos = ampersand;
                            }
                            else
                            {
                                pos = url.end();
                            }
                        }
                    }

                    pos = std::find(url.begin(), url.end(), '?');
                    if (pos != url.end())
                    {
                        url.erase(pos, url.end());
                    }
                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::enqueue(
                        std::unique_ptr<responses::IRequestResponseOperation> response)
                {
                    operations.emplace_back(std::move(response));
                    if (!current_operation)
                    {
                        send_first_part();
                    }
                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::send_first_part()
                {
                    if (operations.size() > 0)
                    {
                        current_operation = std::move(operations.front());
                        operations.pop_front();

                        std::unordered_map<std::string, std::string> headers{};
                        current_operation->get_headers(headers);

                        std::vector<uint8_t> data{};
                        auto res = current_operation->get_data(ContentChuckSize, data);

                        HTTPPacket p{current_operation->get_response_code(), "1.1", headers, data};
                        auto& tx = this->get_buffers()->get_tx_buffer();
                        tx.put(p);

                        if (res == responses::ResponseStatus::AllSent)
                        {
                            current_operation.reset();
                        }
                    }
                }
            }
        }
    }
}