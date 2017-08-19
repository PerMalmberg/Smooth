//
// Created by permal on 8/18/17.
//

#undef write
#include <driver/spi_master.h>
#include <smooth/core/ipc/Mutex.h>
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
                        virtual core::ipc::Mutex& get_guard() const = 0;

                    private:
                };
            }
        }
    }
}
