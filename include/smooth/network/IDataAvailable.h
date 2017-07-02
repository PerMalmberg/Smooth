//
// Created by permal on 7/2/17.
//

#pragma once

#include "Socket.h"

namespace smooth
{
    namespace network
    {
        class Socket;

        class IDataAvailable
        {
            virtual void data_available(Socket& socket ) = 0;
        };
    }
}
