#pragma once

namespace secure_socket_test
{

    class HTTPPacket
            : public smooth::core::network::IPacketDisassembly
    {
        public:
            HTTPPacket()
                    : content(1)
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
            { return static_cast<int>(content.size()); };

            // Must return a pointer to the data to be sent.
            const uint8_t* get_data() override
            { return content.data(); };

            const std::string& get_status_line() const { return status_line; }

            std::unordered_map <std::string, std::string> headers{};
            std::vector <uint8_t> content;
            std::string status_line{};
    };

}