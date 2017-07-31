//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/UnsubAck.h>

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
                    void UnsubAck::visit( IPacketReceiver& receiver )
                    {
                        receiver.receive(*this);
                    }
                }
            }
        }
    }
}
