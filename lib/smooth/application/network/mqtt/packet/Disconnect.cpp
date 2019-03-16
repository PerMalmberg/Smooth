//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/Disconnect.h>

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
                            : MQTTProtocol()
                    {
                        set_header(PacketType::DISCONNECT, 0);
                        encode_remaining_length(0);
                    }
                }
            }
        }
    }
}