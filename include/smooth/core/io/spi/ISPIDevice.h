//
// Created by permal on 8/18/17.
//

#undef write
#include <driver/spi_master.h>
#include <mutex>
namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace spi
            {
                class ISPIDevice
                {
                    public:
                        virtual ~ISPIDevice()
                        {
                        }

                        virtual bool write(spi_transaction_t& transaction) = 0;
                        virtual std::mutex& get_guard() const = 0;

                    private:
                };
            }
        }
    }
}
