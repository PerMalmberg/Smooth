#include <smooth/application/network/http/HTTPServerClient.h>

namespace smooth::application::network::http
{
    static const char* tag = "HTTPServerClient";

    void HTTPServerClient::event(
            const core::network::event::DataAvailableEvent<HTTPProtocol>& event)
    {
        typename HTTPProtocol::packet_type packet;
        if (event.get(packet))
        {
            bool first_packet = !packet.is_continuation();
            bool res = true;

            if (first_packet)
            {
                // First packet, parse URL etc.
                request_headers.clear();
                std::swap(request_headers, packet.headers());
                requested_url = packet.get_request_url();
                res = parse_url(requested_url);
                set_keep_alive();

                mime.reset();
            }

            if (res)
            {
                auto* context = reinterpret_cast<IRequestHandler*>(
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
                                        mime,
                                        first_packet,
                                        !packet.is_continued());
                    }
                    else
                    {
                        // Unsupported method.
                        reply(std::make_unique<responses::StringResponse>(ResponseCode::Method_Not_Allowed));
                    }
                }
            }
        }
    }


    void
    HTTPServerClient::event(
            const smooth::core::network::event::TransmitBufferEmptyEvent&)
    {
        if (current_operation)
        {
            std::vector<uint8_t> data;
            auto res = current_operation->get_data(content_chunk_size, data);

            if (res == responses::ResponseStatus::Error)
            {
                Log::error(tag, "Current operation reported error, closing server client.");
                this->close();
            }
            else if (res == responses::ResponseStatus::EndOfData)
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


    void HTTPServerClient::reply(
            std::unique_ptr<responses::IRequestResponseOperation> response)
    {
        using namespace std::chrono;
        const auto timeout = duration_cast<seconds>(this->socket->get_receive_timeout());
        if (timeout.count() > 0)
        {
            response->add_header(CONNECTION, "keep-alive");
            response->add_header(KEEP_ALIVE, "timeout=" + std::to_string(timeout.count()));
        }

        operations.emplace_back(std::move(response));
        if (!current_operation)
        {
            send_first_part();
        }
    }

    void HTTPServerClient::reply_error(std::unique_ptr<responses::IRequestResponseOperation> response)
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
        if (!operations.empty())
        {
            current_operation = std::move(operations.front());
            operations.pop_front();

            const auto& headers = current_operation->get_headers();

            std::vector<uint8_t> data{};
            auto res = current_operation->get_data(content_chunk_size, data);

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

                if (res == responses::ResponseStatus::EndOfData)
                {
                    current_operation.reset();
                    // Immediately send next
                    send_first_part();
                }
            }
        }
    }


    bool HTTPServerClient::translate_method(
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


    void HTTPServerClient::set_keep_alive()
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