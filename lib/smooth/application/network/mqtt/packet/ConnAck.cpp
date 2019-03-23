//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/ConnAck.h>
#include <smooth/application/network/mqtt/packet/IPacketReceiver.h>

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
                    void ConnAck::visit( IPacketReceiver& receiver )
                    {
                        receiver.receive(*this);
                    }
                }
            }
        }
    }
}
