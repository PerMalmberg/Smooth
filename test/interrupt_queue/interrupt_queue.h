#pragma once

#include <iostream>
#include <smooth/core/Application.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/io/InterruptInput.h>
#include <smooth/core/ipc/ISRTaskEventQueue.h>
#include <smooth/core/ipc/IEventListener.h>

namespace interrupt_queue
{
    class App
            : public smooth::core::Application,
              public smooth::core::ipc::IEventListener<smooth::core::io::InterruptInputEvent>
    {
        public:
            App();

            void init() override;

            void event(const smooth::core::io::InterruptInputEvent& value) override;

        protected:
            smooth::core::ipc::ISRTaskEventQueue<smooth::core::io::InterruptInputEvent, 5> queue;
            smooth::core::io::InterruptInput input;
    };
}