
#include <smooth/application/network/http/responses/NotFound_404.h>

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
                    ResponseCode NotFound_404::get_response_code()
                    {
                        return ResponseCode::Not_Found;
                    }

                    void NotFound_404::get_headers(std::unordered_map<std::string, std::string>& headers)
                    {
                        headers["Connection"] = "Close";
                    }


                    ResponseStatus NotFound_404::get_data(std::size_t /*max_amount*/, std::vector<uint8_t>& /*target*/)
                    {
                        return ResponseStatus::AllSent;
                    }
                }
            }
        }
    }
}