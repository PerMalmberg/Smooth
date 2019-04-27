#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <smooth/core/network/IPacketDisassembly.h>

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

                        enum class Method
                        {
                                GET,
                                POST
                        };

                        HTTPPacket() = default;

                        HTTPPacket(const HTTPPacket&) = default;

                        HTTPPacket& operator=(const HTTPPacket&) = default;

                        explicit HTTPPacket(Method method, const std::string& path,
                                            const std::unordered_map<std::string, std::string>& headers);

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

                        void set_status(int code)
                        {
                            status_code = code;
                        }

                        int status() const
                        {
                            return status_code;
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

                        int empty_space() const
                        {
                            return static_cast<int>(content.size() -
                                                    static_cast<decltype(content)::size_type>(bytes_received));
                        }

                        void increase_size(int increase)
                        {
                            content.resize(content.size() + static_cast<decltype(content)::size_type>(increase));
                        }

                        std::unordered_map<std::string, std::string>& headers()
                        {
                            return header;
                        }

                        void data_received(int length)
                        {
                            bytes_received += length;
                        }

                        int get_bytes_received() const
                        {
                            return bytes_received;
                        }

                        void clear()
                        {
                            content.clear();
                            bytes_received = 0;
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

                        std::unordered_map<std::string, std::string> header{};
                        std::vector<uint8_t> content{};
                        int status_code = 0;
                        int bytes_received = 0;
                        bool continuation = false;
                        bool continued = false;

                        static constexpr const char* const http_get = "GET";
                        static constexpr const char* const http_post = "POST";
                        static constexpr const char* const http_1_0 = "HTTP/1.0";
                };
            }
        }
    }
}