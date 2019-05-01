
#include <smooth/application/network/http/responses/OK_200.h>
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
                    OK_200::OK_200(std::string&& content)
                    {
                        data.insert(data.end(), std::make_move_iterator(content.begin()),
                                    std::make_move_iterator(content.end()));
                        content.erase(content.begin(), content.end());
                    }

                    ResponseCode OK_200::get_response_code()
                    {
                        return ResponseCode::OK;
                    }


                    void OK_200::get_headers(std::unordered_map<std::string, std::string>& headers)
                    {
                        headers["Connection"] = "Close";
                    }

                    ResponseStatus OK_200::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
                    {
                        auto res = ResponseStatus::AllSent;

                        auto remaining = std::distance(data.begin(), data.end());
                        if (remaining > 0)
                        {
                            auto to_send = std::min(static_cast<std::size_t>(remaining), max_amount);
                            auto begin = data.begin();
                            auto end = data.begin() + static_cast<long>(to_send);

                            std::copy(std::make_move_iterator(begin), std::make_move_iterator(end), std::back_inserter(target));
                            data.erase(begin, end);

                            // Anything still left?
                            remaining = std::distance(data.begin(), data.end());
                            res = remaining > 0 ? ResponseStatus::HasMoreData : ResponseStatus::AllSent;
                        }

                        return res;
                    }
                }
            }
        }
    }
}