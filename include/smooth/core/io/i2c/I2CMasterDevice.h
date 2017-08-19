//
// Created by permal on 8/19/17.
//

#pragma once

#include <memory>
#include <vector>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include <smooth/core/ipc/Mutex.h>
#include <smooth/core/util/make_unique.h>

#undef read
#undef write

namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace i2c
            {
                /// Base class for all I2C master devices.
                class I2CMasterDevice
                {

                    public:

                        /// Constructor
                        /// \param port The port
                        /// \param guard The guard mutex.
                        I2CMasterDevice(i2c_port_t port, core::ipc::Mutex& guard)
                                : port(port), guard(guard)
                        {
                        }

                        virtual ~I2CMasterDevice()
                        {
                        }

                    protected:
                        /// Writes the data in the vector to the slave with the provided address.
                        /// Usually the data consists of one or more pairs of register and data bytes.
                        /// \param address The slave address.
                        /// \param data The data to write
                        /// \param enable_ack if true, expect ACK from slave.
                        /// \return true on success, false,
                        bool write(uint8_t address, std::vector<uint8_t>& data, bool enable_ack);

                        /// Reads data from the register of the slave with the provided address.
                        /// The amount of data read is the same as the amount of data currently being
                        /// held by the vector, so fill that with the number of bytes to read.
                        /// \param address The slave address
                        /// \param slave_register The register to read from.
                        /// \param data Where the data will be written to.
                        /// \return true on success, false on failure.
                        bool read(uint8_t address, uint8_t slave_register, std::vector<uint8_t>& data);

                    private:
                        i2c_port_t port;
                        core::ipc::Mutex& guard;
                };
            }
        }
    }
}