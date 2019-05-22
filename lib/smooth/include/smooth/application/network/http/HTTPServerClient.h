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
                static const char* tag = "HTTPServerClient";
                static const std::chrono::milliseconds DefaultKeepAlive{5000};

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

                        bool translate_method(const HTTPPacket& packet, HTTPMethod& method);

                        std::unordered_map<std::string, std::string> request_parameters{};
                        std::unordered_map<std::string, std::string> request_headers{};
                        std::string requested_url{};
                        URLEncoding encoding{};
                        std::deque<std::unique_ptr<responses::IRequestResponseOperation>> operations{};
                        std::unique_ptr<responses::IRequestResponseOperation> current_operation{};

                        void set_keep_alive();
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
                            set_keep_alive();
                        }

                        if (res)
                        {
                            auto* context = reinterpret_cast<IRequestHandler<MaxHeaderSize, ContentChuckSize>*>(
                                    this->get_client_context());

                            if (context)
                            {
                                HTTPMethod method{};
                                if (translate_method(packet, method))
                                {
                                    context->handle(method,
                                                    *this,
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

                        if (res == responses::ResponseStatus::Error)
                        {
                            Log::error(tag, "Current operation reported error, closing server client.");
                            this->close();
                        }
                        else if(res == responses::ResponseStatus::AllSent)
                        {
                            current_operation.reset();
                            // Immediately send next
                            send_first_part();
                        }
                        else
                        {
                            HTTPPacket p{data};
                            auto& tx = this->get_buffers()->get_tx_buffer();
                            tx.put(p);
                        }
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
                    this->socket->set_receive_timeout(DefaultKeepAlive);
                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::reset_client()
                {
                    operations.clear();
                    current_operation.reset();
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
                    using namespace std::chrono;
                    const auto timeout = duration_cast<seconds>(this->socket->get_receive_timeout());
                    if (timeout.count() > 0)
                    {
                        response->add_header("connection", "keep-alive");
                        response->add_header("keep-alive", "timeout=" + std::to_string(timeout.count()));
                    }

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

                        const auto& headers = current_operation->get_headers();

                        std::vector<uint8_t> data{};
                        auto res = current_operation->get_data(ContentChuckSize, data);

                        if (res == responses::ResponseStatus::Error)
                        {
                            Log::error(tag, "Current operation reported error, closing server client.");
                            this->close();
                        }
                        else
                        {
                            // Whether or not everything is sent, send the current (possibly header-only) packet.
                            HTTPPacket p{current_operation->get_response_code(), "1.1", headers, data};
                            auto& tx = this->get_buffers()->get_tx_buffer();
                            tx.put(p);

                            if(res == responses::ResponseStatus::AllSent)
                            {
                                current_operation.reset();
                                // Immediately send next
                                send_first_part();
                            }
                        }
                    }
                }

                template<int MaxHeaderSize, int ContentChuckSize>
                bool HTTPServerClient<MaxHeaderSize, ContentChuckSize>::translate_method(
                        const smooth::application::network::http::HTTPPacket& packet,
                        smooth::application::network::http::HTTPMethod& method)
                {
                    auto res = true;

                    // HTTP verbs are case sensitive: https://tools.ietf.org/html/rfc7230#section-3.1.1
                    if (packet.get_request_method() == "POST")
                    {
                        method = HTTPMethod::POST;
                    }
                    else if (packet.get_request_method() == "GET")
                    {
                        method = HTTPMethod::GET;
                    }
                    else if (packet.get_request_method() == "DELETE")
                    {
                        method = HTTPMethod::DELETE;
                    }
                    else if (packet.get_request_method() == "HEAD")
                    {
                        method = HTTPMethod::HEAD;
                    }
                    else if (packet.get_request_method() == "PUT")
                    {
                        method = HTTPMethod::PUT;
                    }
                    else
                    {
                        res = false;
                    }

                    return res;
                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::set_keep_alive()
                {
                    auto connection = request_headers.find("connection");
                    if (connection != request_headers.end())
                    {
                        auto s = (*connection).second;
                        if (s.find("keep-alive") != std::string::npos)
                        {
                            this->socket->set_receive_timeout(DefaultKeepAlive);
                        }
                    }
                }
            }
        }
    }
}