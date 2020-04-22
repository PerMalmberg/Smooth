#include "smooth/application/network/http/regular/HTTPRequestHandler.h"
#include "smooth/application/network/http/regular/HTTPHeaderDef.h"

namespace smooth::application::network::http::regular
{
    void HTTPRequestHandler::prepare_mime()
    {
        mime.reset();

        // In case the expected headers don't exist, catch any exceptions.
        try
        {
            mime.detect_mode(headers().at(CONTENT_TYPE),
                            std::stoul(headers().at(CONTENT_LENGTH)));
        }
        catch (...)
        {
            // Ignore
        }
    }

    void HTTPRequestHandler::update_call_params(bool first_part,
                                                bool last_part,
                                                IServerResponse& response,
                                                const std::unordered_map<std::string, std::string>& headers,
                                                const std::unordered_map<std::string, std::string>& request_parameters)
    {
        request_params.first_part = first_part;
        request_params.last_part = last_part;
        request_params.response = &response;
        request_params.headers = &headers;
        request_params.request_parameters = &request_parameters;
    }
}
