#pragma once

#include <memory>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                class IResponseQueue
                {
                    public:
                        virtual ~IResponseQueue() = default;

                        virtual void enqueue(std::unique_ptr<responses::IRequestResponseOperation> response) = 0;
                };
            }
        }
    }
}