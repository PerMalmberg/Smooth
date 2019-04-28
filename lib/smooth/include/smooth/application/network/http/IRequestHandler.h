#pragma once

#include "ResponseSignature.h"

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                class IRequestHandler
                {
                    public:
                        virtual ~IRequestHandler() = default;

                        virtual void on_post(const std::string&& url, ResponseSignature response) = 0;
                };
            }
        }
    }
}