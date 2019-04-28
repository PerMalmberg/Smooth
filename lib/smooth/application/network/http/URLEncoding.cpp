
#include <smooth/application/network/http/URLEncoding.h>
#include <cctype>
#include <sstream>
#include <array>
#include <algorithm>

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
                    return decode(url, url.begin(), url.end());
                }

                bool URLEncoding::has_two_left(std::size_t total_length, std::size_t current_ix) const
                {
                    return (total_length - current_ix) >= 3;
                }

                bool URLEncoding::decode(std::string& url, std::string::iterator begin, std::string::iterator end)
                {
                    bool res = true;
                    std::array<char, 3> hex{0, 0, 0};

                    // We know that the result string is going to be shorter or equal to the original,
                    // so we'll do an in-place conversion to save on memory usage.

                    auto write = begin;
                    auto read = begin;

                    for (; res && read != end;)
                    {
                        if (*read == '%')
                        {
                            if (std::distance(read, end) > 2)
                            {
                                hex[0] = *(++read);
                                hex[1] = *(++read);
                                ++read;

                                auto a = std::isxdigit(static_cast<unsigned char>(hex[0]));
                                auto b = std::isxdigit(static_cast<unsigned char>(hex[1]));

                                res = a && b;

                                if (res)
                                {
                                    try
                                    {
                                        *(write++) = static_cast<char>(std::stoul(hex.data(), nullptr, 16));
                                    }
                                    catch (...)
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
                            *write = *read;
                            ++read;
                            ++write;
                        }
                    }

                    if (res)
                    {
                        *write = '\0';
                        url.erase(write, end);
                    }

                    return res;
                }

            }
        }
    }
}
