#include "UploadResponder.h"
#include "smooth/application/network/http/regular/HTTPHeaderDef.h"
#include "smooth/application/network/http/regular/MIMEParser.h"
#include "smooth/core/filesystem/filesystem.h"

namespace http_server_test
{
    using namespace smooth::application::network::http;
    using namespace smooth::application::network::http::regular;
    using namespace smooth::core::filesystem;

    /// This request handler assumed it is being used only to handle either form or URL encoded data in a POST.
    /// As such, it forwards all data to the MIMEParser.
    void UploadResponder::request(IConnectionTimeoutModifier& timeout_modifier,
                                  const std::string& url,
                                  const std::vector<uint8_t>& content)
    {
        (void)timeout_modifier;
        (void)url;

        // Pass content to mime parser with callbacks to handle the data.
        mime.parse(content, *this, *this);
    }

    void UploadResponder::form_data(
        const std::string& field_name,
        const std::string& actual_file_name,
        const smooth::application::network::http::regular::BoundaryIterator& begin,
        const smooth::application::network::http::regular::BoundaryIterator& end)
    {
        // Store the file in web_root/uploads
        Path path{ uploads / ("[" + field_name + "]" + actual_file_name) };
        create_directory(path.parent());
        File to_save{ path };

        if (FileInfo{ path }.exists())
        {
            remove(path);
        }

        auto len = std::distance(begin, end);
        to_save.write(&*begin, static_cast<int>(len));

        if (is_last())
        {
            response().reply(std::make_unique<responses::StringResponse>(ResponseCode::OK,
                                                    "File have been stored in "
                                                    + uploads.str()), false);
        }
    }

    void UploadResponder::url_encoded(std::unordered_map<std::string, std::string>& data)
    {
        if (is_last())
        {
            response().reply(
                std::make_unique<responses::StringResponse>( ResponseCode::OK,
                R"(You entered this text:<br/> <textarea readonly cols="120" rows="20" wrap="soft">)"
                + data["edit_box"] + "</textarea>"),
                false);
        }
    }
}
