#pragma once

#include "Response.h"
#include <smooth/core/filesystem/Path.h>

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
                    class FileContentResponse
                            : public Response
                    {
                        public:
                            explicit FileContentResponse(smooth::core::filesystem::Path full_path);

                            // Called once when beginning to send a response.
                            void get_headers(std::unordered_map<std::string, std::string>& headers) override;

                            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
                            ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;

                        private:
                            std::string get_content_type();

                            smooth::core::filesystem::Path path;
                            const std::int64_t file_size{0};
                            std::int64_t sent{0};
                    };
                }
            }
        }
    }
}