#include <smooth/core/sntp/Sntp.h>

#include <lwip/apps/sntp.h>
#include <ctime>
#include <iostream>

namespace smooth
{
    namespace core
    {
        namespace sntp
        {
            Sntp::Sntp(const std::vector<std::string>& servers)
                    : servers(servers)
            {
            }

            void Sntp::start()
            {
                if(!started)
                {
                    sntp_setoperatingmode(SNTP_OPMODE_POLL);

                    uint8_t i = 0;
                    for (auto& s : servers)
                    {
                        // Only a pointer to the string is stored in the underlying structs.
                        sntp_setservername(i++, const_cast<char *>(s.c_str()));
                    }

                    sntp_init();
                }
            }

            bool Sntp::is_time_set() const
            {
                time_t now{};
                tm timeinfo{};
                time(&now);
                localtime_r(&now, &timeinfo);

                return timeinfo.tm_year > 70;
            }


        }
    }
}
