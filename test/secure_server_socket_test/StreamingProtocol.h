#pragma once

#include <array>
#include <smooth/core/network/IPacketDisassembly.h>
#include <smooth/core/network/IPacketAssembly.h>
#include "StreamPacket.h"

namespace secure_server_socket_test
{
    class StreamingProtocol
            : public smooth::core::network::IPacketAssembly<StreamingProtocol, StreamPacket>
    {
        public:
            using packet_type = StreamPacket;

            int get_wanted_amount(StreamPacket& /*packet*/) override
            {
                return 1;
            }

            void data_received(StreamPacket& /*packet*/, int /*length*/) override
            {
                complete = true;
            }

            uint8_t* get_write_pos(StreamPacket& packet) override
            {
                return packet.data().data();
            }

            bool is_complete(StreamPacket& /*packet*/) override
            {
                return complete;
            }

            bool is_error() override
            {
                // Can't fail when there really is no actual protocol.
                return false;
            }

            void packet_consumed() override
            {
                complete = false;
            }

        private:
            bool complete{false};
    };
}