//
// Created by permal on 7/22/17.
//

#include <limits>
#include <smooth/application/network/mqtt/packet/PingReq.h>

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
                    PingReq::PingReq()
                            : MQTTPacket()
                    {
                        set_header(PacketType::PINGREQ, 0);
                        encode_remaining_length(0);
                    }
                }
            }
        }
    }
}