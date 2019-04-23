#pragma once

#include <cstdint>
#include <smooth/core/network/IPacketDisassembly.h>
#include <smooth/core/network/IPacketAssembly.h>
#include "HTTPPacket.h"

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                class HTTPProtocol
                        : public smooth::core::network::IPacketAssembly<HTTPProtocol, HTTPPacket>
                {
                    public:
                        using packet_type = HTTPPacket;

                        explicit HTTPProtocol(HTTPPacket& working_packet)
                                : packet(working_packet)
                        {
                        }

                        int get_wanted_amount(HTTPPacket& /*packet*/) override
                        {
                            return 1;
                        }

                        void data_received(HTTPPacket& /*packet*/, int /*length*/) override
                        {
                            complete = true;
                        }

                        uint8_t* get_write_pos(HTTPPacket& /*packet*/) override
                        {
                            return nullptr;
                        }

                        bool is_complete() override
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
                        HTTPPacket& packet;
                };
            }
        }
    }
}


