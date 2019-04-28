#pragma once

#include <smooth/core/network/ServerClient.h>
#include <smooth/application/network/http/HTTPProtocol.h>
#include "IRequestHandler.h"
#include "URLEncoding.h"

#include <iostream>
#include <fstream>

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
                        : public smooth::core::network::ServerClient<HTTPServerClient<MaxHeaderSize, ContentChuckSize>, HTTPProtocol<MaxHeaderSize, ContentChuckSize>>
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
                        event(const smooth::core::network::event::DataAvailableEvent<HTTPProtocol<MaxHeaderSize, ContentChuckSize>>& /*event*/) override;

                        void event(const smooth::core::network::event::TransmitBufferEmptyEvent& /*event*/) override;

                        void disconnected() override;

                        void connected() override;

                        void reset_client() override;

                        std::chrono::milliseconds get_send_timeout() override
                        {
                            return std::chrono::seconds{1};
                        }

                    private:
                        bool parse_url(std::string& raw_url);

                        void separate_request_parameters(std::string& url);

                        std::unordered_map<std::string, std::string> request_parameters{};
                        std::unordered_map<std::string, std::string> request_headers{};
                        std::string requested_url{};
                        URLEncoding encoding{};
                };

                template<int MaxHeaderSize, int ContentChuckSize>
                void HTTPServerClient<MaxHeaderSize, ContentChuckSize>::event(
                        const core::network::event::DataAvailableEvent<HTTPProtocol<MaxHeaderSize, ContentChuckSize>>& event)
                {
                    typename HTTPProtocol<MaxHeaderSize, ContentChuckSize>::packet_type packet;
                    if (event.get(packet))
                    {
                        bool first_packet = !packet.is_continuation();
                        //bool last_packet = !packet.is_continued();
                        bool res = true;

                        if (first_packet)
                        {
                            // First packet, parse URL
                            request_headers.clear();
                            std::swap(request_headers, packet.headers());
                            requested_url = std::move(packet.get_request_url());
                            res = parse_url(requested_url);
                        }
                        else
                        {
                            // Pass previously parsed URL, query parameters and headers in call.
                        }


                        if (res)
                        {

                            /*auto context = this->get_client_context<IRequestHandler>();
                            if(context)
                            {
                                context->on_post()
                            } */
                        }
                    }
                }

                template<int MaxHeaderSize, int ContentChuckSize>
                void
                HTTPServerClient<MaxHeaderSize, ContentChuckSize>::event(
                        const smooth::core::network::event::TransmitBufferEmptyEvent&)
                {
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
                }
            }
        }
    }
}