#include "UploadResponder.h"
#include "smooth/application/network/http/regular/HTTPHeaderDef.h"
#include "smooth/application/network/http/regular/MIMEParser.h"
#include "smooth/core/filesystem/filesystem.h"

namespace http_server_test
{
    using namespace smooth::application::network::http;
    using namespace smooth::application::network::http::regular;
    using namespace smooth::core::filesystem;

    void UploadResponder::request(IServerResponse& response,
                                  IConnectionTimeoutModifier& timeout_modifier,
                                  const std::string& url,
                                  bool first_part,
                                  bool last_part,
                                  const std::unordered_map<std::string, std::string>& headers,
                                  const std::unordered_map<std::string, std::string>& request_parameters,
                                  const std::vector<uint8_t>& content)
    {
        (void)timeout_modifier;
        (void)headers;
        (void)request_parameters;
        (void)url;

        if (first_part)
        {
            // Prepare mime parser to receive data
            // QQQ mime.detect_mode(headers.at(CONTENT_TYPE), std::stoul(headers.at(CONTENT_LENGTH)));
        }

        auto form_data = [this, &last_part, &response](const std::string& name,
                                                       const std::string& actual_file_name,
                                                       const MIMEParser::BoundaryIterator& begin,
                                                       const MIMEParser::BoundaryIterator& end)
                         {
                             // Store the file in web_root/uploads
                             Path path{ uploads / ("[" + name + "]" + actual_file_name) };
                             create_directory(path.parent());
                             File to_save{ path };

                             if (FileInfo{ path }.exists())
                             {
                                 remove(path);
                             }

                             auto len = std::distance(begin, end);
                             to_save.write(&*begin, static_cast<int>(len));

                             if (last_part)
                             {
                                 response.reply(std::make_unique<responses::StringResponse>(ResponseCode::OK,
                                                                           "File have been stored in "
                                                                           + uploads.str()), false);
                             }
                         };

        auto url_encoded_data =
            [&response, &last_part](std::unordered_map<std::string, std::string>& data)
            {
                if (last_part)
                {
                    response.reply(std::make_unique<responses::StringResponse>( ResponseCode::OK,
                R"(You entered this text:<br/> <textarea readonly cols="120" rows="20" wrap="soft">)"
                    + data["edit_box"] + "</textarea>"),
                    false);
                }
            };

        // Pass content to mime parser with callbacks to handle the data.
        // QQQ mime.parse(content, form_data, url_encoded_data);
    }
}
