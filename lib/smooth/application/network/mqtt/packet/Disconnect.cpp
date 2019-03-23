//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/Disconnect.h>
#include <smooth/application/network/mqtt/packet/IPacketReceiver.h>

using namespace std;

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
                    Disconnect::Disconnect()
                            : MQTTPacket()
                    {
                        set_header(PacketType::DISCONNECT, 0);
                        encode_remaining_length(0);
                    }
                }
            }
        }
    }
}