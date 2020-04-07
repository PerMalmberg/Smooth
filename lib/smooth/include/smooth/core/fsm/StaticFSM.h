/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <cstdlib>
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::core::fsm
{
    /// StaticFSM is a memory-static implementation of a Finite State Machine.
    /// It consumes StateSize * 2 bytes of memory at all times.
    /// All states used with this FSM must support enter_state and leave_state methods.
    /// These are called after in such a way that they do not overlap the way constructors
    /// and destructors do when switching between states.
    ///
    /// You need to subclass from this to provide the BaseState and StateSize template arguments.
    ///
    /// \tparam BaseState The base state of all states used by the instance of the FSM
    /// \tparam StateSize The maximum size, in bytes, of the largest state used by the FSM.
    /// i.e. sizeof(LargestState). The reason that this has to be specified is that states are
    /// created using placement new in a pre-allocated memory area.
    template<typename BaseState, std::size_t StateSize>
    class StaticFSM
    {
        public:
            StaticFSM() = default;

            virtual ~StaticFSM();

            /// Sets the new state.
            /// Inside the states, you use the following syntax to change to a new state.
            /// fsm.set_state( new(fsm) NameOfNewState(fsm) )
            /// Note: After setting the new state, only variables on the stack are alive, any class members
            /// will have been destructed.
            /// \param state
            void set_state(BaseState* state);

            void* select_memory_area(size_t size);

        protected:
            /// Called as a notification to the subclassing FSM before a new state is entered.
            /// \param state The new state
            virtual void entering_state(BaseState*)
            {
            }

            /// Called as a notification to the subclassing FSM before the current state is left.
            /// \param state The current state
            virtual void leaving_state(BaseState*)
            {
            }

            /// Gets the current state
            /// \return The current state
            BaseState* get_state() const
            {
                return current_state;
            }

        private:
#pragma pack(push, 1)
            uint8_t state[2][static_cast<size_t>(StateSize)]{};
#pragma pack(pop)
            BaseState* current_state = nullptr;
    };

    template<typename BaseState, std::size_t StateSize>
    StaticFSM<BaseState, StateSize>::~StaticFSM()
    {
        // Destroy any currently active state
        if (current_state)
        {
            current_state->~BaseState();
        }
    }

    template<typename BaseState, std::size_t StateSize>
    void* StaticFSM<BaseState, StateSize>::select_memory_area(size_t size)
    {
        if (size > StateSize)
        {
            Log::error("StaticFSM", "Attempted to activate state that is larger ({}) than the designated buffer ({})",
                              size, StateSize);
            abort();
        }

        // Get the memory not used by the active state.
        void* reclaimed =
            current_state == reinterpret_cast<void*>(&state[0][0]) ? &state[1][0] : &state[0][0];

        return reclaimed;
    }

    template<typename BaseState, std::size_t StateSize>
    void StaticFSM<BaseState, StateSize>::set_state(BaseState* new_state)
    {
        if (current_state != nullptr)
        {
            leaving_state(current_state);
            current_state->leave_state();
            current_state->~BaseState();
        }

        current_state = new_state;
        entering_state(current_state);
        current_state->enter_state();
    }
}

template<typename BaseState, std::size_t StateSize>
void* operator new(size_t size, smooth::core::fsm::StaticFSM<BaseState, StateSize>& fsm)
{
    // Return the memory area to use for placement new.
    return fsm.select_memory_area(size);
}

template<typename BaseState, std::size_t StateSize>
void operator delete(void*, smooth::core::fsm::StaticFSM<BaseState, StateSize>& fsm)
{
    fsm.select_memory_area(0);
}
