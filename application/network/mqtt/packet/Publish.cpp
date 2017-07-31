//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/Publish.h>

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
                    void Publish::visit( IPacketReceiver& receiver )
                    {
                        receiver.receive(*this);
                    }
                }
            }
        }
    }
}
