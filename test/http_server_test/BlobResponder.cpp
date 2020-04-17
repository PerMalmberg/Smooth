#include "BlobResponder.h"
#include "SendBlob.h"

namespace http_server_test
{
    using namespace smooth::application::network::http;
    using namespace smooth::application::network::http::regular;

    void BlobResponder::request(IServerResponse& response,
                                IConnectionTimeoutModifier& timeout_modifier,
                                const std::string& url,
                                bool first_part,
                                bool last_part,
                                const std::unordered_map<std::string, std::string>& headers,
                                const std::unordered_map<std::string, std::string>& request_parameters,
                                const std::vector<uint8_t>& content,
                                MIMEParser& mime)
    {
        (void)timeout_modifier;
        (void)url;
        (void)first_part;
        (void)headers;
        (void)content;
        (void)mime;

        if (last_part)
        {
            std::size_t size = 0;

            try
            {
                size = std::stoul(request_parameters.at("size"));
            }
            catch (...)
            {
            }

            if (size == 0)
            {
                response.reply(std::make_unique<responses::StringResponse>(ResponseCode::Expectation_Failed,
                                                                "Request parameter 'size' must be > 0"),
                                                                false);
            }
            else
            {
                response.reply(std::make_unique<SendBlob>(size), false);
            }
        }
    }
}
