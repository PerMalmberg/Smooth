#pragma once

class StreamPacket
        : public smooth::core::network::IPacketDisassembly
{
    public:
        int get_send_length() override
        {
            return buff.size();
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