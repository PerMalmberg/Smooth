/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/************************************************************************************
   SPECIAL NOTES                SPECIAL NOTES                          SPECIAL NOTES

    The AxpPMU provides a set of the most common functions that can be used to interact
    with an AXP device such as the AXP192, AXP173 or AXP202.
    These functions allow the the user to:
        1. Write - A single bit, multiple bits or a single byte
        2. Read  - A single bit, multiple bits, a single byte or multiple bytes
        3. Read  - Signals captured by the ADC and get the calulated results
        4. Read  - The calculated battery capacity

    ---------------------------------------------------------------------------------
    Pin         Axp173                  Axp192                  Axp202
    ---------------------------------------------------------------------------------
    DC1         0.7V-3.5V @1.2A         0.7V-3.5V @ 1.2A        Not Available
    DC2         0.7V-2.275V @ 1.6A      0.7V-2.275V @ 1.6A      0.7V-2.275V @ 1.6A
    DC3         Not Available           0.7V-3.5V @ 700mA       0.7V-3.5V @ 1.2A
    LDO1        3.3V @ 30mA             3.3V @ 30mA             3.3V @ 30mA
    LDO2        1.8V-3.3V @ 200mA       1.8V-3.3V @ 200mA       1.8V-3.3V @ 200mA
    LDO3        1.8V-3.3V @ 200mA       1.8V-3.3V @ 200mA       0.7V-3.3V @ 200mA
    LDO4        0.7V-3.5V @ 500mA       Not Available           1.8V-3.3V @ 200mA
    LDO5io0     Not Available           1.8V-3.3V @ 50mA        1.8V-3.3V @ 50mA
*************************************************************************************/
#pragma once

#include "smooth/core/io/i2c/I2CMasterDevice.h"
#include "smooth/application/io/i2c/AxpRegisters.h"

namespace smooth::application::sensor
{
    class AxpPMU : public core::io::i2c::I2CMasterDevice
    {
        public:
            /// Constructor
            AxpPMU(i2c_port_t port, uint8_t address, std::mutex& guard);

            /////////////////////////////////////////////////////////////////////////////////
            // Core read and write functions
            /////////////////////////////////////////////////////////////////////////////////

            /// Write a byte to a register
            /// \param reg The register to write to
            /// \param data_byte The data byte to write to the register
            /// \return true on success, false on failure.
            bool write_register(AxpRegister reg, uint8_t data_byte);

            /// Read a byte from register
            /// \param reg The register to read
            /// \param read_data Holds the data byte read
            /// \return true on success, false on failure.
            bool read_register(AxpRegister reg, uint8_t& read_data);

            /// Write a register bit
            /// \param reg The register that will be written
            /// \param bit The bit value to write
            /// \param bit_postion The bit position of bit we want to write
            /// \return true on success, false on failure.
            bool write_register_bit(AxpRegister reg, bool bit, uint8_t bit_position);

            /// Read a register bit
            /// \param reg The register that will be read
            /// \param bit Holds the value of the bit read
            /// \param bit_postion The bit position of bit we want to read
            /// \return true on success, false on failure.
            bool read_register_bit(AxpRegister reg, bool& bit, uint8_t bit_position);

            /// Read an adjoining group of bits from register
            /// \param reg The register to read
            /// \param read_data Holds the bits read
            /// \param mask The bits we want to clear are set to 0, bits we want to stay the same are set to 1
            /// \param right_shift_count After masking the read_data will shifted right this number of bits
            /// \return true on success, false on failure.
            bool read_register_bits(AxpRegister reg, uint8_t mask, uint8_t right_shift_count, uint8_t& read_data);

            /// Write an adjoining group of bits to a register
            /// \param reg The register to write
            /// \param write_data The data to write
            /// \param mask The bits we want to clear are set to 0, bits we want to stay the same are set to 1
            /// \param left_shift_count After masking the read_data will shifted left this number of bits
            /// \return true on success, false on failure.
            bool write_register_bits(AxpRegister reg, uint8_t mask, uint8_t left_shift_count, uint8_t write_data);

            /// Write init registers to AXP device - used to configure (initialize) the axp device
            /// Each product that uses an AXP device will probably require the axp device to be configured uniquely
            /// uniquely per product requirements.  This function provide a fast and easy way to send registers
            /// with data to configure the axp device.  Typically axp_init_reg_t will be placed in a std::array
            /// container and pass the pointer to the first element and the arry size to this function.
            /// \param init_regs The pointer to the first byte in init_regs
            /// \param length The number of axp_init_reg's
            /// \return true on success, false on failure.
            bool write_init_regs(const AxpInitReg* init_regs, size_t length);

            /// Read 12 bits
            /// \param start_reg The starting register to begin the read
            /// \param data The 12 bits are located in b11-b0; b15-b12 are 0
            /// \return true on success, false on failure.
            bool read_12_bits(AxpRegister start_reg, uint16_t& data);

            /// Read 13 bits
            /// \param start_reg The starting register to begin the read
            /// \param data The 13 bits are located in b12-b0; b15-b13 are 0
            /// \return true on success, false on failure.
            bool read_13_bits(AxpRegister start_reg, uint16_t& data);

            /// Read 24 bits
            /// \param start_reg The starting register to begin the read
            /// \param data The 24 bits are located in b23-b0; b31-b24 are 0
            /// \return true on success, false on failure.
            bool read_24_bits(AxpRegister start_reg, uint32_t& data);

            /// Read 32 bits
            /// \param start_reg The starting register to begin the read
            /// \param data The 32 bits
            /// \return true on success, false on failure.
            bool read_32_bits(AxpRegister start_reg, uint32_t& data);

            /////////////////////////////////////////////////////////////////////////////////
            // ADC data functions
            /////////////////////////////////////////////////////////////////////////////////

            /// Get ACIN voltage
            /// \param acin_voltage The ACIN voltage in volts
            /// \return true on success, false on failure.
            bool get_acin_voltage(float& acin_voltage);

            /// Get ACIN current
            /// \param acin_current The ACIN current in mA
            /// \return true on success, false on failure.
            bool get_acin_current(float& acin_current);

            /// Get VBUS voltage
            /// \param vbus_voltage The supply voltage from the USB source in volts
            /// \return true on success, false on failure.
            bool get_vbus_voltage(float& vbus_voltage);

            /// Get VBUS current
            /// \param vbus_current The current from the USB source
            /// \return true on success, false on failure.
            bool get_vbus_current(float& vbus_current);

            /// Get the internal temperature of the AXP device
            /// \param device_temp The internal temperature of the AXP device in degree celsius
            /// \return true on success, false on failure.
            bool get_axp_device_temperature(float& device_temp);

            /// Get TS voltage (battery temperature sensor voltage)
            /// \param ts_voltage The TS voltage in volts
            /// \return true on success, false on failure.
            bool get_ts_voltage(float& ts_voltage);

            // Get GPIO0 voltage
            /// \param gpio_voltage The GPIO0 voltage
            /// \return true on success, false on failure.
            virtual bool get_gpio0_voltage(float& gpio_voltage);

            /// Get GPIO1 voltage
            /// \param gpio_voltage The GPIO1 voltage
            /// \return true on success, false on failure.
            virtual bool get_gpio1_voltage(float& gpio_voltage);

            /// Get GPIO2 voltage
            /// \param gpio_voltage The GPIO2 voltage
            /// \return true on success, false on failure.
            virtual bool get_gpio2_voltage(float& gpio_voltage);

            /// Get GPIO3 voltage
            /// \param gpio_voltage The GPIO3 voltage
            /// \return true on success, false on failure.
            virtual bool get_gpio3_voltage(float& gpio_voltage);

            /// Get Battery Power (Instantaneous)
            /// \param batt_power
            /// \return true on success, false on failure.
            bool get_battery_power(float& mw_batt_power);

            /// Get Battery voltage
            /// \param batt_voltage The battery voltage in volts
            /// \return true on success, false on failure.
            bool get_battery_voltage(float& batt_voltage);

            /// Get Battery charging current
            /// \param batt_chrg_current The battery charging current in mA
            /// \return true on success, false on failure.
            bool get_battery_charging_current(float& batt_chrg_current);

            /// Get Battery discharging current
            /// \param batt_dischrg_current The battery discharging current in mA
            /// \return true on success, false on failure.
            bool get_battery_discharging_current(float& batt_dischrg_current);

            /// Get APS voltage
            /// \param aps_voltage The APS voltage in volts
            /// \return true on success, false on failure.
            bool get_aps_voltage(float& aps_voltage);

            /////////////////////////////////////////////////////////////////////////////////
            // Coulomb data functions
            /////////////////////////////////////////////////////////////////////////////////

            /// Get battery capacity in mAh
            /// The current in milliamps which can be sustained by the battery for an hour
            /// \param mah_batt_capacity Battery capacity in mAh
            /// \return true on success, false on failure.
            bool get_battery_capacity(float& mah_batt_capacity);

            /// Clear coulomb counter
            /// \return true on success, false on failure.
            bool clear_coulomb_counter();

            /// Suspend coulomb counter
            /// \return true on success, false on failure.
            bool suspend_coulomb_counter();

        private:
            /// Calculate the GPIO voltage (when used as a ADC input)
            /// \param gpio_volt_adc_reg The GPIO voltage ADC register
            /// \param gpio_num The GPIO number; 0-3
            /// \param voltage Holds the calculated voltage
            /// \return true on success, false on failure.
            bool calc_gpio_input_voltage(AxpRegister gpio_volt_adc_reg, uint8_t gpio_num, float& voltage);
    };
}
