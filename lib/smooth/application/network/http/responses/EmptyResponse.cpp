
#include <smooth/application/network/http/responses/EmptyResponse.h>
#include <smooth/application/network/http/HTTPHeaderDef.h>

namespace smooth::application::network::http::responses
{
    EmptyResponse::EmptyResponse(ResponseCode code)
            : smooth::application::network::http::responses::StringResponse(code)
    {
        headers[CONTENT_LENGTH] = "0";
    }
}