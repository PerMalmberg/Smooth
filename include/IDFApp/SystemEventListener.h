//
// Created by permal on 6/25/17.
//

#pragma once

#include <IDFApp/Application.h>

namespace idfapp
{

    class SystemEventListener
    {
        public:
            SystemEventListener(Application& app)
                    : application(app)
            {
                application.subscribe(this);
            }

            virtual ~SystemEventListener()
            {
                application.unsubscribe(this);
            }

            virtual void system_event(Application& app, system_event_t& event) = 0;
        private:
            Application& application;
    };

}