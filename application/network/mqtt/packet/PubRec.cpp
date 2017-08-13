//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/PubRec.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                namespace packet
                {
                    void PubRec::visit(IPacketReceiver& receiver)
                    {
                        receiver.receive(*this);
                    }

                    uint16_t PubRec::get_packet_identifier() const
                    {
                        calculate_remaining_length_and_variable_header_offset();
                        return read_packet_identifier(variable_header_start);
                    }

                }
            }
        }
    }
}
