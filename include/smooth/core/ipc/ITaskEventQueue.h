//
// Created by permal on 6/27/17.
//

#pragma once

namespace smooth
{
    namespace  core
    {
        namespace ipc
        {
            class ITaskEventQueue
            {
                public:
                    virtual void forward_to_task() = 0;
                    virtual int size() = 0;
                    virtual QueueHandle_t get_handle() = 0;
            };
        }
    }
}
