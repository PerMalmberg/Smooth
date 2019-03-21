#pragma once

namespace secure_socket_test
{

    class HTTPPacket
            : public smooth::core::network::IPacketDisassembly
    {
        public:
            HTTPPacket()
                    : content()
            {

            }

            HTTPPacket(const HTTPPacket&) = default;

            HTTPPacket& operator=(const HTTPPacket&) = default;

            explicit HTTPPacket(const std::string& data)
            {
                std::copy(data.begin(), data.end(), std::back_inserter(content));
            }

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

            int content_length() const
            {
                return static_cast<int>(content.size());
            }

            std::vector<uint8_t>& data()
            {
                return content;
            }

            void set_size(int size)
            {
                if(content.size()!=size)
                {
                    content.reserve(static_cast<unsigned long>(size));
                }
            }

            int empty_space() const
            {
                return static_cast<int>(content.size() - bytes_received);
            }

            void increase_size(int increase)
            {
                content.resize(content.size()+increase);
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

            void append_null()
            {
                content.push_back(0);
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
            std::unordered_map<std::string, std::string> header{};
            std::vector<uint8_t> content{};
            int status_code = 0;
            int bytes_received = 0;
            bool continuation = false;
            bool continued = false;
    };

}