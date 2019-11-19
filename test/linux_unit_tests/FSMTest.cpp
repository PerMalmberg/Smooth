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

#include <catch2/catch.hpp>
#include "smooth/core/fsm/StaticFSM.h"

using namespace smooth::core::fsm;

constexpr std::size_t size = 64;

class MyFsm;

class BaseState
{
    public:
        explicit BaseState(MyFsm& fsm)
                : fsm(fsm), dummy()
        {
        }

        virtual ~BaseState() = default;

        virtual void enter_state() {}

        virtual void leave_state() {}

        virtual std::string name() = 0;

        virtual void f() = 0;

    protected:
        MyFsm& fsm;
        uint8_t dummy[10];
};

class MyFsm : public StaticFSM<BaseState, size>
{
    public:
        void f()
        {
            get_state()->f();
        }

        [[nodiscard]] std::string get_name() const
        {
            return get_state()->name();
        }
};

class A : public BaseState
{
    public:
        explicit A(MyFsm& fsm)
                : BaseState(fsm)
        {
        }

        std::string name() override
        {
            return "A";
        }

        void f() override;
};

class B : public BaseState
{
    public:
        explicit B(MyFsm& fsm)
                : BaseState(fsm)
        {
        }

        std::string name() override
        {
            return "B";
        }

        void f() override
        {
            fsm.set_state(new(fsm) A(fsm));
        }
};

void A::f()
{
    fsm.set_state(new B(fsm));
}

SCENARIO("FSM Test")
{
    MyFsm fsm{};
    fsm.set_state(new(fsm) A(fsm));
    REQUIRE(fsm.get_name() == "A");
    fsm.f();
    REQUIRE(fsm.get_name() == "B");
    fsm.f();
    REQUIRE(fsm.get_name() == "A");
}
