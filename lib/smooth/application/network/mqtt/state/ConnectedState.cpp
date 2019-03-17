//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/ConnectedState.h>
#include <smooth/application/network/mqtt/state/IdleState.h>
#include <smooth/application/network/mqtt/state/DisconnectState.h>
#include <smooth/application/network/mqtt/packet/PingReq.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                namespace state
                {
                    void ConnectedState::event(const core::timer::TimerExpiredEvent& event)
                    {
                        auto timer_id = event.get_id();

                        if (timer_id == MQTT_FSM_KEEP_ALIVE_TIMER_ID)
                        {
                            packet::PingReq ping;
                            //QQQ fsm.get_mqtt().send_packet(ping);
                        }
                    }

                    void ConnectedState::event(const core::network::ConnectionStatusEvent& event)
                    {
                        if (!event.is_connected())
                        {
                            fsm.set_state(new(fsm) IdleState(fsm));
                        }
                    }
                }
            }
        }
    }
}