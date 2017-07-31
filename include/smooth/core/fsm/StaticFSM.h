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

            // All states used with this FSM must support enter_state and leave_state methods.
            // These are called after in such a way that they do not overlap the way constructors
            // and destructors do when switching between states.

            template<typename BaseState, int StateSize>
            class StaticFSM
            {
                public:
                    StaticFSM() = default;
                    virtual ~StaticFSM();

                    void set_state(BaseState* state);

                    void* select_memory_area(int size);

                    virtual void entering_state(BaseState* state)
                    {
                    }

                    virtual void leaving_state(BaseState* state)
                    {
                    }

                    BaseState* get_state() const
                    {
                        return current_state;
                    }

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
            void* StaticFSM<BaseState, StateSize>::select_memory_area(int size)
            {
                int max = static_cast<int>( sizeof(state[0]));
                if (size > max)
                {
                    ESP_LOGE("StaticFSM",
                             "Attempted to activate state that is larger (%d) than the designated buffer (%d)",
                             size,
                             max);
                    abort();
                }

                // Get the memory not used by the active state.
                void* reclaimed =
                        current_state == reinterpret_cast<BaseState*>(&state[0][0]) ? &state[1][0] : &state[0][0];

                return reclaimed;
            }

            template<typename BaseState, int StateSize>
            void StaticFSM<BaseState, StateSize>::set_state(BaseState* state)
            {
                if (current_state != nullptr)
                {
                    leaving_state(current_state);
                    current_state->leave_state();
                    current_state->~BaseState();
                }

                current_state = state;
                entering_state(current_state);
                current_state->enter_state();
            }
        }
    }
}

template<typename BaseState, int StateSize>
void* operator new(size_t size, smooth::core::fsm::StaticFSM<BaseState, StateSize>& fsm)
{
    // Return the memory are to use for placement new.
    return fsm.select_memory_area(size);
}


template<typename BaseState, int StateSize>
void operator delete(void*, smooth::core::fsm::StaticFSM<BaseState, StateSize>& fsm)
{
    fsm.select_memory_area(0);
}
