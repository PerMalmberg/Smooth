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
#include <math.h>
#include "smooth/application/io/i2c/AxpPMU.h"
#include "smooth/core/util/FixedBuffer.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core;
using namespace smooth::core::util;
using namespace smooth::core::logging;

namespace smooth::application::sensor
{
    // Constructor
    AxpPMU::AxpPMU(i2c_port_t port, uint8_t address, std::mutex& guard)
            : I2CMasterDevice(port, address, guard)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    // Core read and write functions
    /////////////////////////////////////////////////////////////////////////////////

    // Write a data byte to a register
    bool AxpPMU::write_register(AxpRegister reg, uint8_t write_data)
    {
        std::vector<uint8_t> data{ static_cast<uint8_t>(reg) };
        data.push_back(write_data);

        return write(address, data);
    }

    // Read a byte from register
    bool AxpPMU::read_register(AxpRegister reg, uint8_t& read_data)
    {
        util::FixedBuffer<uint8_t, 1> rd_data;
        bool res = read(address, static_cast<uint8_t>(reg), rd_data);
        read_data = rd_data[0];

        return res;
    }

    // Write (set/clear) a bit in a register
    bool AxpPMU::write_register_bit(AxpRegister reg, bool bit, uint8_t bit_position)
    {
        uint8_t read_data;
        bool res = read_register(reg, read_data)
                   & write_register(reg,
        static_cast<uint8_t>((read_data & ~(1 << bit_position)) | bit << bit_position));

        return res;
    }

    // Read register bit
    bool AxpPMU::read_register_bit(AxpRegister reg, bool& bit, uint8_t bit_position)
    {
        uint8_t read_data;
        bool res = read_register(reg, read_data);
        bit = (read_data & (1 << bit_position)) != 0;

        return res;
    }

    // Write adjoining group of bits to register
    bool AxpPMU::write_register_bits(AxpRegister reg, uint8_t mask, uint8_t left_shift_count, uint8_t write_data)
    {
        uint8_t rd_data;
        bool res = read_register(reg, rd_data)
                   & write_register(reg, static_cast<uint8_t>((rd_data & mask) | (write_data << left_shift_count)));

        return res;
    }

    // Write init_regs to initialize the axp device
    bool AxpPMU::write_init_regs(const AxpInitReg* init_regs, size_t length)
    {
        bool res = true;

        // check res everytime thru loop
        for (size_t index = 0; res && index < length; index++)
        {
            res = write_register(init_regs[index].reg, init_regs[index].data);
        }

        return res;
    }

    // Read adjoining group of bits from register
    bool AxpPMU::read_register_bits(AxpRegister reg, uint8_t mask, uint8_t right_shift_count, uint8_t& read_data)
    {
        uint8_t rd_data;
        bool res = read_register(reg, rd_data);
        read_data = static_cast<uint8_t>((rd_data & mask) >> right_shift_count);

        return res;
    }

    // Read 12 bits
    bool AxpPMU::read_12_bits(AxpRegister start_reg, uint16_t& data)
    {
        FixedBuffer<uint8_t, 2> rd_buf;
        bool res = read(address, static_cast<uint8_t>(start_reg), rd_buf);
        data = static_cast<uint16_t>(rd_buf[0] << 4 | rd_buf[1]);

        return res;
    }

    // Read 13 bits
    bool AxpPMU::read_13_bits(AxpRegister start_reg, uint16_t& data)
    {
        FixedBuffer<uint8_t, 2> rd_buf;
        bool res = read(address, static_cast<uint8_t>(start_reg), rd_buf);
        data = static_cast<uint16_t>(rd_buf[0] << 5 | rd_buf[1]);

        return res;
    }

    // Read 24 bits
    bool AxpPMU::read_24_bits(AxpRegister start_reg, uint32_t& data)
    {
        FixedBuffer<uint8_t, 3> rd_buf;
        bool res = read(address, static_cast<uint8_t>(start_reg), rd_buf);
        data = static_cast<uint32_t>(rd_buf[0] << 16 | rd_buf[1] << 8 | rd_buf[2]);

        return res;
    }

    // Read 32 bits
    bool AxpPMU::read_32_bits(AxpRegister start_reg, uint32_t& data)
    {
        FixedBuffer<uint8_t, 4> rd_buf;
        bool res = read(address, static_cast<uint8_t>(start_reg), rd_buf);
        data = static_cast<uint32_t>(rd_buf[0] << 24 | rd_buf[1] << 16 | rd_buf[2] << 8 | rd_buf[3]);

        return res;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // ADC data
    /////////////////////////////////////////////////////////////////////////////////

    // Get the ACIN voltage - range = 0V to 6.9615V
    bool AxpPMU::get_acin_voltage(float& acin_voltage)
    {
        uint16_t step_count;
        bool res = read_12_bits(AxpRegister::Reg56H_Acin_Volt_HI8, step_count);
        acin_voltage = static_cast<float>(step_count * 0.0017);

        return res;
    }

    // Get the ACIN current - range = 0mA to 2.5594A
    bool AxpPMU::get_acin_current(float& acin_current)
    {
        uint16_t step_count;
        bool res = read_12_bits(AxpRegister::Reg58H_Acin_Curr_HI8, step_count);
        acin_current = static_cast<float>(step_count * 0.625);

        return res;
    }

    // Get the VBUS voltage - range = 0V to 6.9615V
    bool AxpPMU::get_vbus_voltage(float& vbus_voltage)
    {
        uint16_t step_count;
        bool res = read_12_bits(AxpRegister::Reg5AH_Vbus_Volt_HI8, step_count);
        vbus_voltage = static_cast<float>(step_count * 0.0017);

        return res;
    }

    // Get the VBUS current - range = 0mA to 1.5356A
    bool AxpPMU::get_vbus_current(float& vbus_current)
    {
        uint16_t step_count;
        bool res = read_12_bits(AxpRegister::Reg5CH_Vbus_Curr_HI8, step_count);
        vbus_current = static_cast<float>(step_count * 0.375);

        return res;
    }

    // Get the internal temperature of the AXP device - range = -144.7C to +264.8C
    bool AxpPMU::get_axp_device_temperature(float& device_temp)
    {
        uint16_t step_count;
        bool res = read_12_bits(AxpRegister::Reg5EH_Intr_Temp_HI8, step_count);
        device_temp = static_cast<float>(-144.7 + (step_count * 0.1));

        return res;
    }

    // Get battery temperature sensor voltage - range 0V to 3.276V
    bool AxpPMU::get_ts_voltage(float& ts_voltage)
    {
        uint16_t step_count;
        bool res = read_12_bits(AxpRegister::Reg62H_Batt_Temp_HI8, step_count);
        ts_voltage = static_cast<float>(step_count * 0.0008);

        return res;
    }

    // Get GPIO0 voltage (when used as ADC) - range 0V to 2.0475V or 0.7V to 2.7475V
    bool AxpPMU::get_gpio0_voltage(float& gpio_voltage)
    {
        return calc_gpio_input_voltage(AxpRegister::Reg64H_Gpio0_Volt_HI8, 0, gpio_voltage);
    }

    // Get GPIO1 voltage (when used as ADC) - range 0V to 2.0475V or 0.7V to 2.7475V
    bool AxpPMU::get_gpio1_voltage(float& gpio_voltage)
    {
        return calc_gpio_input_voltage(AxpRegister::Reg64H_Gpio0_Volt_HI8, 1, gpio_voltage);
    }

    // Get GPIO2 voltage (when used as ADC)- range 0V to 2.0475V or 0.7V to 2.7475V
    bool AxpPMU::get_gpio2_voltage(float& gpio_voltage)
    {
        return calc_gpio_input_voltage(AxpRegister::Reg64H_Gpio0_Volt_HI8, 2, gpio_voltage);
    }

    // Get GPIO3 voltage (when used as ADC) - range 0V to 2.0475V or 0.7V to 2.7475V
    bool AxpPMU::get_gpio3_voltage(float& gpio_voltage)
    {
        return calc_gpio_input_voltage(AxpRegister::Reg64H_Gpio0_Volt_HI8, 3, gpio_voltage);
    }

    // Get battery power - in milliwatts
    bool AxpPMU::get_battery_power(float& mw_batt_power)
    {
        uint32_t power_count;
        bool res = read_24_bits(AxpRegister::Reg70H_Batt_Power_HI8, power_count);
        mw_batt_power = static_cast<float>((power_count * 0.5 * 1.1) / 1000);

        return res;
    }

    // Get battery voltage - range = 0mV to 4.5045V
    bool AxpPMU::get_battery_voltage(float& batt_voltage)
    {
        uint16_t step_count;
        bool res = read_12_bits(AxpRegister::Reg78H_Batt_Volt_HI8, step_count);
        batt_voltage = static_cast<float>(step_count * 0.0011);

        return res;
    }

    // Get battery charging current in mA
    bool AxpPMU::get_battery_charging_current(float& batt_chrg_current)
    {
        uint16_t step_count;
        bool res = read_13_bits(AxpRegister::Reg7AH_Batt_Chrg_Curr_HI8, step_count);
        batt_chrg_current = static_cast<float>(step_count * 0.5);

        return res;
    }

    // Get battery discharging current in mA
    bool AxpPMU::get_battery_discharging_current(float& batt_dischrg_current)
    {
        uint16_t step_count;
        bool res = read_13_bits(AxpRegister::Reg7CH_Batt_DisChg_Curr_HI8, step_count);
        batt_dischrg_current = static_cast<float>(step_count * 0.5);

        return res;
    }

    // Get APS voltage - range = 0mV to 5.733V
    // Approx 5V when connected to USB, and 4V when connected to battery
    bool AxpPMU::get_aps_voltage(float& aps_voltage)
    {
        uint16_t step_count;
        bool res = read_12_bits(AxpRegister::Reg7EH_Aps_Voltage_HI8, step_count);
        aps_voltage = static_cast<float>(step_count * 0.0014);

        return res;
    }

    // Calculate GPIO input voltage (when used as ADC)
    bool AxpPMU::calc_gpio_input_voltage(AxpRegister gpio_volt_adc_reg, uint8_t gpio_num, float& voltage)
    {
        uint16_t step_count;
        bool is_range_0v7_to_2V7;

        bool res = read_12_bits(gpio_volt_adc_reg, step_count)
                   & read_register_bit(AxpRegister::Reg85H_Adc_Input_Range, is_range_0v7_to_2V7, gpio_num);

        if (is_range_0v7_to_2V7)
        {
            voltage = static_cast<float>(0.7 + (step_count * 0.0005));
        }
        else
        {
            voltage = static_cast<float>(step_count * 0.0005);
        }

        return res;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // Coulomb data
    /////////////////////////////////////////////////////////////////////////////////

    // Get battery capacity in mAh
    bool AxpPMU::get_battery_capacity(float& mah_batt_capacity)
    {
        uint32_t chrg_coulomb;
        uint32_t dischrg_coulomb;
        uint8_t adc_sample_rate;

        bool res = read_32_bits(AxpRegister::RegB0H_Batt_Chrg_Coulomb3, chrg_coulomb)
                   & read_32_bits(AxpRegister::RegB4H_Batt_DisChg_Coulomb3, dischrg_coulomb)
                   & read_register_bits(AxpRegister::Reg84H_Adc_Sample_Rate, 0xC0, 6, adc_sample_rate);

        adc_sample_rate = static_cast<uint8_t>(25 * pow(2, adc_sample_rate));
        mah_batt_capacity =
            static_cast<float>(((65536 * 0.5 * (chrg_coulomb - dischrg_coulomb)) / adc_sample_rate) / 3600);

        return res;
    }

    // Clear coulomb counter
    bool AxpPMU::clear_coulomb_counter()
    {
        return write_register_bit(AxpRegister::RegB8H_Coulomb_Counter_Ctrl, true, 5);
    }

    // Suspend coulomb counter
    bool AxpPMU::suspend_coulomb_counter()
    {
        return write_register_bit(AxpRegister::RegB8H_Coulomb_Counter_Ctrl, true, 6);
    }
}
