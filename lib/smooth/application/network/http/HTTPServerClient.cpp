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

#include "smooth/application/network/http/HTTPServerClient.h"
#include "smooth/application/network/http/IResponseOperation.h"
#include "smooth/application/network/http/websocket/responses/WSResponse.h"

namespace smooth::application::network::http
{
    static constexpr const char* tag = "HTTPServerClient";
    using namespace websocket::responses;
    using namespace websocket;

    void HTTPServerClient::event(
        const core::network::event::DataAvailableEvent<HTTPProtocol>& event)
    {
        if (mode == Mode::HTTP)
        {
            http_event(event);
        }
        else
        {
            websocket_event(event);
        }
    }

    void HTTPServerClient::event(
        const smooth::core::network::event::TransmitBufferEmptyEvent&)
    {
        if (current_operation)
        {
            std::vector<uint8_t> data;
            auto res = current_operation->get_data(content_chunk_size, data);

            if (res == ResponseStatus::Error)
            {
                Log::error(tag, "Current operation reported error, closing server client.");
                this->close();
            }
            else if (res == ResponseStatus::NoData)
            {
                current_operation.reset();

                // Immediately send next
                send_first_part();
            }
            else if (res == ResponseStatus::HasMoreData
                     || res == ResponseStatus::LastData)
            {
                HTTPPacket p{ data };
                auto& tx = this->container->get_tx_buffer();
                tx.put(p);
            }
        }
        else
        {
            send_first_part();
        }
    }

    void HTTPServerClient::disconnected()
    {
    }

    void HTTPServerClient::connected()
    {
        this->socket->set_receive_timeout(DefaultKeepAlive);
    }

    void HTTPServerClient::reset_client()
    {
        operations.clear();
        current_operation.reset();
        mode = Mode::HTTP;
        ws_server.reset();
    }

    bool HTTPServerClient::parse_url(std::string& raw_url)
    {
        separate_request_parameters(raw_url);

        auto res = encoding.decode(raw_url);

        return res;
    }

    void HTTPServerClient::separate_request_parameters(std::string& url)
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
                    auto value = std::string{ ++equal_sign, ampersand };
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

    void HTTPServerClient::reply(
        std::unique_ptr<IResponseOperation> response, bool place_first)
    {
        if (mode == Mode::HTTP)
        {
            using namespace std::chrono;
            const auto timeout = duration_cast<seconds>(this->socket->get_receive_timeout());

            if (timeout.count() > 0)
            {
                response->add_header(CONNECTION, "keep-alive");
                response->set_header(KEEP_ALIVE, "timeout=" + std::to_string(timeout.count()));
            }
        }

        if (operations.size() >= max_enqueued_responses)
        {
            // To prevent a build up of unsent responses, which all consume a bit of memory
            // (f.ex. echoing incoming data) we don't let the total amount of operations grow beyond
            // the set number.
            Log::error(tag, "Overflow protection triggered.");
            close();
        }
        else
        {
            if (place_first)
            {
                operations.insert(operations.begin(), std::move(response));
            }
            else
            {
                operations.emplace_back(std::move(response));
            }

            if (!current_operation)
            {
                send_first_part();
            }
        }
    }

    void HTTPServerClient::reply_error(std::unique_ptr<IResponseOperation> response)
    {
        operations.clear();
        response->add_header(CONNECTION, "close");
        operations.emplace_back(std::move(response));

        if (!current_operation)
        {
            send_first_part();
        }
    }

    void HTTPServerClient::send_first_part()
    {
        ResponseStatus res = ResponseStatus::Error;

        do
        {
            if (!operations.empty())
            {
                current_operation = std::move(operations.front());
                operations.pop_front();

                const auto& headers = current_operation->get_headers();

                std::vector<uint8_t> data{};
                res = current_operation->get_data(content_chunk_size, data);

                if (res == ResponseStatus::Error)
                {
                    Log::error(tag, "Current operation reported error, closing server client.");
                    this->close();
                }
                else
                {
                    auto& tx = this->container->get_tx_buffer();
                    auto buffer_consumed_data = false;

                    if (mode == Mode::HTTP)
                    {
                        // Whether or not everything is sent, send the current (possibly header-only) packet.
                        HTTPPacket p{ current_operation->get_response_code(), "1.1", headers, data };
                        buffer_consumed_data = tx.put(p);
                    }
                    else
                    {
                        HTTPPacket p{ data };
                        buffer_consumed_data = tx.put(p);
                    }

                    if (!buffer_consumed_data
                        || res == ResponseStatus::NoData)
                    {
                        current_operation.reset();
                    }
                }
            }
        }
        while (!operations.empty()
               && res == ResponseStatus::NoData); // Process next operation as long as no data is sent.
    }

    bool HTTPServerClient::translate_method(
        const smooth::application::network::http::HTTPPacket& packet,
        smooth::application::network::http::HTTPMethod& method) const
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

    void HTTPServerClient::set_keep_alive()
    {
        auto connection = request_headers.find("connection");

        if (connection != request_headers.end())
        {
            auto s = (*connection).second;

            if (string_util::icontains(s, "keep-alive"))
            {
                this->socket->set_receive_timeout(DefaultKeepAlive);
            }
        }
    }

    void HTTPServerClient::http_event(const core::network::event::DataAvailableEvent<HTTPProtocol>& event)
    {
        typename HTTPProtocol::packet_type packet;

        if (event.get(packet))
        {
            bool first_packet = !packet.is_continuation();
            bool last_packet = !packet.is_continued();

            bool res = true;

            if (first_packet)
            {
                // First packet, parse URL etc.
                request_headers.clear();
                std::swap(request_headers, packet.headers());
                requested_url = packet.get_request_url();
                res = parse_url(requested_url);
                set_keep_alive();
            }

            if (res)
            {
                auto* context = this->get_client_context();

                if (context)
                {
                    HTTPMethod method{};

                    if (translate_method(packet, method))
                    {
                        context->handle(method,
                                        *this,
                                        *this,
                                        requested_url,
                                        request_headers,
                                        request_parameters,
                                        packet.get_buffer(),
                                        first_packet,
                                        last_packet);
                    }
                    else
                    {
                        // Unsupported method.
                        reply(std::make_unique<regular::responses::StringResponse>(ResponseCode::Method_Not_Allowed),
                              false);
                    }
                }
            }
        }
    }

    void HTTPServerClient::websocket_event(const smooth::core::network::event::DataAvailableEvent<HTTPProtocol>& event)
    {
        typename HTTPProtocol::packet_type packet;

        if (event.get(packet))
        {
            auto ws_op = packet.ws_control_code();

            if (ws_op >= OpCode::Close)
            {
                if (ws_op == OpCode::Close)
                {
                    close();
                }
                else if (ws_op == OpCode::Ping)
                {
                    // Reply with a Pong and place it first in the queue.
                    reply(std::make_unique<WSResponse>(OpCode::Pong), true);
                }
            }
            else
            {
                if (ws_server)
                {
                    bool first_part = !packet.is_continuation();
                    bool last_part = !packet.is_continued();
                    const auto& data = packet.data();
                    ws_server->data_received(first_part, last_part, packet.ws_control_code() == OpCode::Text, data);
                }
            }
        }
    }
}
