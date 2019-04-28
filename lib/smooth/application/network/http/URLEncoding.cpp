
#include <smooth/application/network/http/URLEncoding.h>
#include <cctype>
#include <sstream>
#include <array>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                bool http::URLEncoding::decode(std::string& url)
                {
                    bool res = true;
                    std::array<char,3> hex{ 0, 0, 0};

                    // We know that the result string is going to be shorter or equal to the original,
                    // so we'll do an in-place conversion to save on memory usage.
                    auto org_size = url.size();

                    std::size_t write = 0;

                    for(std::size_t read = 0; res && read < org_size;)
                    {
                        if(url[read] == '%')
                        {
                            if(has_two_left(org_size, read))
                            {
                                hex[0] = url[++read];
                                hex[1] = url[++read];
                                ++read;

                                auto a = std::isxdigit(static_cast<unsigned char>(hex[0]));
                                auto b = std::isxdigit(static_cast<unsigned char>(hex[1]));

                                res = a && b;

                                if(res)
                                {
                                    try
                                    {
                                        url[write++] = static_cast<char>(std::stoul(hex.data(), nullptr, 16));
                                    }
                                    catch(...)
                                    {
                                        res = false;
                                    }
                                }
                            }
                            else
                            {
                                res = false;
                            }
                        }
                        else
                        {
                            // Just keep going
                            url[write] = url[read];
                            ++read;
                            write++;
                        }
                    }

                    if(res)
                    {
                        url[write] = '\0';
                        url.erase(url.begin() + static_cast<long>(write), url.end());
                    }

                    return res;
                }

                bool URLEncoding::has_two_left(std::size_t total_length, std::size_t current_ix) const
                {
                    return (total_length - current_ix) >= 3;
                }

            }
        }
    }
}
