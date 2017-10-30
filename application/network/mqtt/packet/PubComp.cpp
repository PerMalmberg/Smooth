//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/PubComp.h>

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
                    void PubComp::visit( IPacketReceiver& receiver )
                    {
                        receiver.receive(*this);
                    }

                    uint16_t PubComp::get_packet_identifier() const
                    {
                        calculate_remaining_length_and_variable_header_offset();
                        return read_packet_identifier(get_variable_header_start());
                    }
                }
            }
        }
    }
}
