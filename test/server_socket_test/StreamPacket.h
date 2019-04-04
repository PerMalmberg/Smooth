#pragma once

class StreamPacket
        : public smooth::core::network::IPacketDisassembly
{
    public:

        StreamPacket() = default;
        StreamPacket(const StreamPacket&) = default;
        StreamPacket& operator=(const StreamPacket&) = default;

        explicit StreamPacket(char c)
        {
            buff[0] = static_cast<unsigned char>(c);
        }

        int get_send_length() override
        {
            return static_cast<int>(buff.size());
        }

        const uint8_t* get_data() override
        {
            return buff.data();
        }

        std::array<uint8_t, 1>& data()
        {
            return buff;
        }

    private:
        std::array<uint8_t, 1> buff{};
};