
#include <smooth/application/network/http/responses/EmptyResponse.h>

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
                    EmptyResponse::EmptyResponse(ResponseCode code)
                            : smooth::application::network::http::responses::Response(code)
                    {
                        headers["content-length"] = "0";
                    }
                }
            }
        }
    }
}