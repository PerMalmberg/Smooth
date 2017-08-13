//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/PubRel.h>

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
                    void PubRel::visit( IPacketReceiver& receiver )
                    {
                        receiver.receive(*this);
                    }

                    uint16_t PubRel::get_packet_identifier() const
                    {
                        calculate_remaining_length_and_variable_header_offset();
                        return read_packet_identifier(variable_header_start);
                    }
                }
            }
        }
    }
}
