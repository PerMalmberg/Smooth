#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <smooth/core/network/IPacketDisassembly.h>
#include <smooth/application/network/http/ResponseCodes.h>
#include "HTTPMethod.h"

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                class HTTPPacket
                        : public smooth::core::network::IPacketDisassembly
                {
                    public:

                        HTTPPacket() = default;

                        HTTPPacket(const HTTPPacket&) = default;

                        HTTPPacket& operator=(const HTTPPacket&) = default;

                        HTTPPacket(HTTPPacket&&) = default;

                        HTTPPacket(ResponseCode code, const std::string& version,
                                   const std::unordered_map<std::string, std::string>& new_headers,
                                   std::vector<uint8_t>& response_content);

                        explicit HTTPPacket(std::vector<uint8_t>& response_content);


                        // Must return the total amount of bytes to send
                        int get_send_length() override
                        {
                            return static_cast<int>(content.size());
                        }

                        // Must return a pointer to the data to be sent.
                        const uint8_t* get_data() override
                        {
                            return content.data();
                        }

                        const auto& get_buffer()
                        {
                            return content;
                        }

                        void set_continued()
                        {
                            continued = true;
                        }

                        bool is_continued() const
                        {
                            return continued;
                        }

                        void set_continuation()
                        {
                            continuation = true;
                        }

                        bool is_continuation() const
                        {
                            return continuation;
                        }

                        void
                        set_request_data(const std::string& method, const std::string& url, const std::string& version)
                        {
                            request_method = method;
                            request_url = url;
                            request_version = version;
                        }

                        const std::string& get_request_url() const
                        {
                            return request_url;
                        }

                        const std::string& get_request_method() const
                        {
                            return request_method;
                        }

                        const std::string& get_request_version() const
                        {
                            return request_version;
                        }

                        std::vector<uint8_t>& data()
                        {
                            return content;
                        }

                        void set_size(int size)
                        {
                            if (content.size() != static_cast<decltype(content)::size_type>(size))
                            {
                                content.reserve(static_cast<decltype(content)::size_type>(size));
                                if (content.size() < static_cast<decltype(content)::size_type>(size))
                                {
                                    content.assign(static_cast<decltype(content)::size_type>(size) - content.size(), 0);
                                }
                            }
                        }

                        void increase_size(int increase)
                        {
                            content.resize(content.size() + static_cast<decltype(content)::size_type>(increase));
                        }

                        std::unordered_map<std::string, std::string>& headers()
                        {
                            return request_headers;
                        }

                        void clear()
                        {
                            content.clear();
                        }

                        bool ends_with_two_crlf() const
                        {
                            // At least one character and \r\n\r\n
                            return content.size() > 4
                                   && content[content.size() - 4] == '\r'
                                   && content[content.size() - 3] == '\n'
                                   && content[content.size() - 2] == '\r'
                                   && content[content.size() - 1] == '\n';
                        }

                    private:
                        void append(const std::string& s);

                        void add_header(const std::string& key, const std::string& value);

                        std::unordered_map<std::string, std::string> request_headers{};
                        std::string request_method{};
                        std::string request_url{};
                        std::string request_version{};
                        std::vector<uint8_t> content{};
                        bool continuation = false;
                        bool continued = false;
                };
            }
        }
    }
}