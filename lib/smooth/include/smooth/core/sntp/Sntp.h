#pragma once

#include <vector>
#include <string>

namespace smooth
{
    namespace core
    {
        namespace sntp
        {
            class Sntp
            {
                public:
                    explicit Sntp(const std::vector<std::string>& servers);

                    void start();
                    bool is_time_set() const;
                private:
                    const std::vector<std::string> servers;
                    bool started = false;
            };
        }
    }
}