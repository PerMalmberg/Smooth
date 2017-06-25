//
// Created by permal on 6/25/17.
//

#pragma once

#include <IDFApp/Application.h>

class SystemEventListener
{
    public:
        SystemEventListener()
        {
            Application::instance().subscribe(this);
        }

        virtual ~SystemEventListener()
        {
            Application::instance().unsubscribe(this);
        }

        virtual void system_event(Application& app, system_event_t& event) = 0;
};
