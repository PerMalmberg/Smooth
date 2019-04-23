#pragma once

namespace smooth
{
    namespace application
    {
        namespace network
        {
            class HTTPPacket : public smooth::core::network::IPacketDisassembly
            {
                public:

                    HTTPPacket() = default;
                    HTTPPacket(const HTTPPacket&) = default;
                    HTTPPacket& operator=(const HTTPPacket&) = default;


                    int get_send_length() override
                    {
                        return 0;
                    }

                    const uint8_t* get_data() override
                    {
                        return nullptr;
                    }

                private:

            };
        }
    }
}