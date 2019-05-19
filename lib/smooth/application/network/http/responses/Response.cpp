
#include <smooth/application/network/http/responses/Response.h>
#include <algorithm>

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
                    Response::Response(ResponseCode code, std::string content)
                            : code(code)
                    {
                        data.insert(data.end(), std::make_move_iterator(content.begin()),
                                    std::make_move_iterator(content.end()));
                        content.erase(content.begin(), content.end());
                    }

                    ResponseCode Response::get_response_code()
                    {
                        return code;
                    }

                    ResponseStatus Response::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
                    {
                        auto res = ResponseStatus::AllSent;

                        auto remaining = std::distance(data.begin(), data.end());
                        if (remaining > 0)
                        {
                            auto to_send = std::min(static_cast<std::size_t>(remaining), max_amount);
                            auto begin = data.begin();
                            auto end = data.begin() + static_cast<long>(to_send);

                            std::copy(std::make_move_iterator(begin), std::make_move_iterator(end),
                                      std::back_inserter(target));
                            data.erase(begin, end);

                            // Anything still left?
                            remaining = std::distance(data.begin(), data.end());
                            res = remaining > 0 ? ResponseStatus::HasMoreData : ResponseStatus::AllSent;
                        }

                        return res;
                    }

                    void Response::add_header(const std::string& key, const std::string& value)
                    {
                        headers[key] = value;
                    }
                }
            }
        }
    }
}