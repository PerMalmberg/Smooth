//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/Unsubscribe.h>

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
                    void Unsubscribe::visit( IPacketReceiver& receiver )
                    {
                        receiver.receive(*this);
                    }
                }
            }
        }
    }
}
