//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/DisconnectedState.h>
#include <smooth/application/network/mqtt/state/ConnectToBrokerState.h>
#include <smooth/application/network/mqtt/Subscription.h>

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
                    void DisconnectedState::enter_state()
                    {
                        fsm.get_mqtt().get_subscription().handle_disconnect();

                        fsm.get_mqtt().set_keep_alive_timer(std::chrono::seconds(0));
                        if (fsm.get_mqtt().is_auto_reconnect())
                        {
                            fsm.get_mqtt().start_reconnect();
                        }
                    }

                    void DisconnectedState::event(const core::timer::TimerExpiredEvent& event)
                    {
                        if (event.get_timer()->get_id() == MQTT_FSM_RECONNECT_TIMER_ID)
                        {
                            fsm.get_mqtt().reconnect();
                        }
                    }

                    void DisconnectedState::event(const core::network::ConnectionStatusEvent& event)
                    {
                        if (fsm.get_mqtt().is_auto_reconnect() && !event.is_connected())
                        {
                            fsm.get_mqtt().start_reconnect();
                        }
                        else if( event.is_connected())
                        {
                            fsm.set_state(new(fsm) ConnectToBrokerState(fsm));
                        }
                    }

                }
            }
        }
    }
}