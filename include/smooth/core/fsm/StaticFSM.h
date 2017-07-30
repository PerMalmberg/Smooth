//
// Created by permal on 7/30/17.
//

#pragma once

#include <stdint.h>
#include "esp_log.h"

namespace smooth
{
    namespace core
    {
        namespace fsm
        {
            // StaticFSM is a memory-static implementation of a Finite State Machine.
            // It consumes StateSize * 2 bytes of memory at all times.

            // All states used with this FSM must support EnterState and LeaveState methods.
            // These are called after in such a way that they do not overlap the way constructors
            // and destructors do when switching between states.

            template<typename BaseState, int StateSize>
            class StaticFSM
            {
                public:
                    StaticFSM() = default;
                    virtual ~StaticFSM();

                    void set_state(BaseState* state);

                    void* reclaim_state(int size);

                private:
                    uint8_t state[2][StateSize];
                    BaseState* current_state = nullptr;
            };


            template<typename BaseState, int StateSize>
            StaticFSM<BaseState, StateSize>::~StaticFSM()
            {
                // Destroy any currently active state
                if (current_state)
                {
                    current_state->~BaseState();
                }
            }

            template<typename BaseState, int StateSize>
            void* StaticFSM<BaseState, StateSize>::reclaim_state(int size)
            {
                int max = static_cast<int>( sizeof(state[0]));
                if (size > max)
                {
                    ESP_LOGE("StaticFSM",
                             "Attempted to activate state that is larger (%d) than the designated buffer (%d)",
                             size,
                             max);
                }

                // Get the memory not currently used
                void* reclaimed =
                        current_state == reinterpret_cast<BaseState*>(&state[0][0]) ? &state[1][0] : &state[0][0];

                return reclaimed;
            }

            template<typename BaseState, int StateSize>
            void StaticFSM<BaseState, StateSize>::set_state(BaseState* state)
            {
                if (current_state != nullptr)
                {
                    current_state->LeaveState();
                    current_state->~BaseState();
                }

                current_state = state;
                current_state->EnterState();
            }
        }
    }
}

template<typename BaseState, int StateSize>
void* operator new(size_t size, smooth::core::fsm::StaticFSM<BaseState, StateSize>& fsm)
{
    return fsm.reclaim_state(size);
}


template<typename BaseState, int StateSize>
void operator delete(void*, smooth::core::fsm::StaticFSM<BaseState, StateSize>& fsm)
{
    fsm.reclaim_state(0);
}
