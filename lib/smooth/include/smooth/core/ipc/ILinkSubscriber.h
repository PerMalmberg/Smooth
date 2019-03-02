//
// Created by permal on 10/26/17.
//

#pragma once

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            template <typename T>
            class ILinkSubscriber
            {
                public:
                    virtual bool receive_published_data(const T& data) = 0;
                    virtual ~ILinkSubscriber() = default;
            };
        }
    }
}
