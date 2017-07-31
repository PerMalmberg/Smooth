//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/SubAck.h>

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
                    void SubAck::visit( IPacketReceiver& receiver )
                    {
                        receiver.receive(*this);
                    }
                }
            }
        }
    }
}
