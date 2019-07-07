// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cstdlib>
#include <smooth/core/logging/log.h>

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
    template<typename BaseState, int StateSize>
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
    void* StaticFSM<BaseState, StateSize>::select_memory_area(size_t size)
    {
        auto max = sizeof(state[0]);

        if (size > max)
        {
            Log::error("StaticFSM",
                       Format("Attempted to activate state that is larger ({1}) than the designated buffer ({2})",
                              UInt64(size),
                              UInt64(max)));
            abort();
        }

        // Get the memory not used by the active state.
        void* reclaimed =
                current_state == reinterpret_cast<void*>(&state[0][0]) ? &state[1][0] : &state[0][0];

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

template<typename BaseState, int StateSize>
void* operator new(size_t size, smooth::core::fsm::StaticFSM<BaseState, StateSize>& fsm)
{
    // Return the memory area to use for placement new.
    return fsm.select_memory_area(size);
}


template<typename BaseState, int StateSize>
void operator delete(void*, smooth::core::fsm::StaticFSM<BaseState, StateSize>& fsm)
{
    fsm.select_memory_area(0);
}

#pragma GCC diagnostic pop
