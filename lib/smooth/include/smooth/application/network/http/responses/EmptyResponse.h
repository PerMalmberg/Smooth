#pragma once

#include "Response.h"

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                namespace responses
                {
                    class EmptyResponse
                            : public Response
                    {
                        public:
                            explicit EmptyResponse(ResponseCode code);
                    };
                }
            }
        }
    }
}