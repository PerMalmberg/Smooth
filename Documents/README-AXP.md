# How to use AxpPMU 
This readme file explains how to use the AxpPMU.cpp associated files in a project. You can use the AxpPMU as a standalone class or you can inherit the class and override some functions and add additional functions.  The AxpPMU class was designed to allow the user easily manipulate a register or registers to configure the device. The following are some of the functions provided by the AxpPMU class.
### Write a byte to a register
bool write_register(AxpRegister reg, uint8_t data_byte);
### Read a byte from register
bool read_register(AxpRegister reg, uint8_t& read_data);
### Write a bit to a register
bool write_register_bit(AxpRegister reg, bool bit, uint8_t bit_position);
### Read a bit from a register
bool read_register_bit(AxpRegister reg, bool& bit, uint8_t bit_position);
### Write an adjoining group of bits to a register
bool write_register_bits(AxpRegister reg, uint8_t mask, uint8_t left_shift_count, uint8_t write_data);
### Read an adjoining group of bits from a register
bool read_register_bits(AxpRegister reg, uint8_t mask, uint8_t right_shift_count, uint8_t& read_data);
### Write initialization registers to configure the chip
bool write_init_regs(const AxpInitReg* init_regs, size_t length);


## 1. An example to use as standalone class
The following is a simple example of using the AxpPMU as a standalone class for the M5StickC.  The M5StickC has the AXP192 chip integrated into its product.  

First create an Axp192Init.h file that is used to configure/initialize the Axp192 on startup. This configuration of the AXP192 is for the M5StickC product other products will probably require a differnt configuration.  
``` C++ code
Axp192Init.h

namespace some_namespace_name
{
    static constexpr std::array<smooth::application::sensor::AxpInitReg, 14> axp192_init_cmds_1 =
    { {
        // #1 -------------------------------------------------------------------------------------
        // Configure LDO2 and LDO3 --- LDO2 = LDO3 = 3.0V
        // b[7:4] = 1100 xxxx = set LDO2 to 3.00V = (3.00-1.8)/0.1 = 12 = 0xC
        // b[3:0] = xxxx 1100 = set LDO3 to 3.00V = (3.00-1.8)/0.1 = 12 = 0xC
        { smooth::application::sensor::AxpRegister::Reg28H_Ldo2_Ldo3_VSet, 0xCC },

        // #2 -------------------------------------------------------------------------------------
        // Configure ADC sampling rate --- ADC sampling rate = 200Hz
        // b[7:6] = 11xx xxxx = set adc sampling rate for 200Hz
        // b[5:4] = xx11 xxxx = set TS output current to 80uA
        // b3     = xxxx 0xxx = reserved or not used
        // b2     = xxxx x0xx = set TS pin function to battery temeprature monitoring function
        // b[1:0] = xxxx xx10 = set TS pin as input when ADC is sampling
        { smooth::application::sensor::AxpRegister::Reg84H_Adc_Sample_Rate, 0xF2 },

        // #3 -------------------------------------------------------------------------------------
        // Configure ADC --- Enable all ADC's
        // b7 = 1xxx xxxx = enable battery voltage ADC
        // b6 = x1xx xxxx = enable battery current ADC
        // b5 = xx1x xxxx = enable ACIN voltage ADC
        // b4 = xxx1 xxxx = enable ACIN current ADC
        // b3 = xxxx 1xxx = enable VBUS voltage ADC
        // b2 = xxxx x1xx = enable VBUS current ADC
        // b1 = xxxx xx1x = enable APS voltage ADC
        // b0 = xxxx xxx1 = enable TS pin ADC
        { smooth::application::sensor::AxpRegister::Reg82H_Adc_Enable_1, 0xFF },

        // #4 -------------------------------------------------------------------------------------
        // Configure battery charging control 1 --- Set charging current to 100mA
        // b7     = 1xxx xxxx = enable battery charger
        // b[6:5] = x10x xxxx = target voltage 4.20V
        // b4     = xxx0 xxxx = end charging current when charging current is <10%
        // b[3:0] = xxxx 0000 = charging current is 100mA
        { smooth::application::sensor::AxpRegister::Reg33H_Chrg_Control_1, 0xC0 },

        // #5 -------------------------------------------------------------------------------------
        // Configure battery charging control 2  --- 
        // b[7:6] = 01xx xxxx = precharge timeout 40 minutes
        // b[5:3] = xx00 0xxx = ext charge current 300ma (not used)
        // b2     = xxxx x0xx = disable external charging
        // b[1:0] = xxxx xx01 = timeout constant current mode 8 hours
        { smooth::application::sensor::AxpRegister::Reg34H_Chrg_Control_2, 0x41 },

        // #6 -------------------------------------------------------------------------------------
        // Configure GPIO_0 pin function  --- Configure GPIO to function as LDO
        // b[7:3] = 0000 0xxx = reserved or not used
        // b[2:0] = xxxx x010 = set GPIO0 pin function to LDO low noise output
        { smooth::application::sensor::AxpRegister::Reg90H_Gpio0_Func_Set, 0x02 },

        // #7 -------------------------------------------------------------------------------------
        // Configure GPIO_0 voltage --- GPIO0 output voltage to 3.3V
        // b[7:4] = 1111 xxxx = set GPIO_0 (LDOio0) voltage to 3.3V
        // b[3:0] = xxxx 0000 = reserved or not used
        { smooth::application::sensor::AxpRegister::Reg91H_Gpio0_Volt_Set, 0xF0 },

        // #8 -------------------------------------------------------------------------------------
        // Configure Power Enable Key (PEK) --- Automatic shutdown when key press is GT 6 seconds
        // b[7:6] = 00xx xxxx = startup time for 128ms
        // b[5:4] = xx00 xxxx = long press time for 1 second
        // b3     = xxxx 1xxx = automatic shutdown when key pressed time exceeds shutdown time is enabled
        // b2     = xxxx x0xx = PWROK signal delay after the power startup for 32mS
        // b[1:0] = xxxx xx01 = shutdown time for 6 seconds
        { smooth::application::sensor::AxpRegister::Reg36H_PEK_Key_Setting, 0x09 }, // change times

        // #9 -------------------------------------------------------------------------------------
        // Configure VBUS-IPSOUT Power Path --- Use VBUS as input power (if useable) regardless of N_VBUSEN
        // b7     = 1xxx xxxx = use VBUS as input power (if useable) regardless of N_VBUSEN
        // b6     = x0xx xxxx = VBUS Vhold voltage is not limited
        // b[5:3] = xx00 0xxx = Vhold = 4.0v (not used because of bit b6)
        // b2     = xxxx x0xx = unchanged or not used
        // b[1:0] = xxxx xx00 = Vbus current limit = 900ma
        { smooth::application::sensor::AxpRegister::Reg30H_Ips_Out_Mngmnt, 0x80 },

        // #10 ------------------------------------------------------------------------------------
        // Configure battery charging high temperature voltage threshold on TS pin
        // b[7:0] = 1111 1100 = voltage threshold = 252 * 16 * 0.0008 = 3.2256V
        { smooth::application::sensor::AxpRegister::Reg39H_Vhth_Chrg_Set, 0xFC },

        // #11 ------------------------------------------------------------------------------------
        // Configure backup battery charge control --- Enable backup battery charging
        // b7     = 1xxx xxxx = enable backup battery charging
        // b[6:5] = x10x xxxx = backup battery charging target voltage for 3.0V
        // b[4:2] = xxx0 00xx = reserved or not used
        // b[1:0] = xxxx xx10 = backup battery charging current for 200uA
        { smooth::application::sensor::AxpRegister::Reg35H_Backup_Chrg_Ctl, 0xA2 },

        // #12 ------------------------------------------------------------------------------------
        // Configure shutdown and Chrg LED --- Do not shutdown
        // b7 = 0xxx xxxx = do not shutdown axp192
        // b6 = x1xx xxxx = enable battery monitoring
        // b[5:4] = xx00 xxxx = set CHGLED pin to high resistance
        // b3     = xxxx 0xxx = CHGLED pin function controlled by bits b[5:4]
        // b2     = xxxx x0xx = reserved or not used
        // b[1:0] = xxxx xx10 = shutdown delay after N_OE changes from low to high
        { smooth::application::sensor::AxpRegister::Reg32H_Shutdn_ChrgLed, 0x42 },

        // #13 ------------------------------------------------------------------------------------
        // Configure coulomb counter --- Enable coulomb counter
        // b7     = 1xxx xxxx = enable coulomb counter
        // b6     = x0xx xxxx = do not suspend coulomb counter
        // b5     = xx0x xxxx = do not stop coulomb counter
        // b[4:0] = reserved not used
        { smooth::application::sensor::AxpRegister::RegB8H_Coulomb_Counter_Ctrl, 0x80 },

        // #14 ------------------------------------------------------------------------------------
        // Configure power output control --- Enable all outputs except DC-DC2
        // b7 = 0xxx xxxx = reserved or not used
        // b6 = x1xx xxxx = enable EXTEN for enabling external DC-DC regulator for HAT
        // b5 = xx0x xxxx = reserved or not used
        // b4 = xxx0 xxxx = disabled DC-DC2 power output
        // b3 = xxxx 1xxx = enable LDO3 power output
        // b2 = xxxx x1xx = enable LDO2 power output
        // b1 = xxxx xx1x = enable DC-DC3 power output
        // b0 = xxxx xxx1 = enable DC-DC1 power output
        { smooth::application::sensor::AxpRegister::Reg12H_Power_Out_Ctrl, 0x4F }
    } };
}
```

Second, create M5StickC.h that uses the Axp192
``` C++ code
M5StickC.h

#pragma once

#include <memory> // for unique_ptr
#include <smooth/core/io/i2c/Master.h>
#include <smooth/application/io/i2c/AxpPMU.h>

namespace some_namespace_name
{
    class M5StickC
    {
        public:
            /// Constructor
            M5StickC();
            
            /// Initialize the M5StickC
            void initialize();

            /// Set screen brightness
            /// \param brightness The brightness level; 0x00=1.8V=Dark, 0x0F=3.3V=Bright
            void set_screen_brightness(uint8_t brightness);

            /// Print the Axp192 measurements
            void print_axp192_report();

        private:
            
            /// Initialize the Axp192 device
            void initialize_axp192();
            
            smooth::core::io::i2c::Master i2c0_master;
            std::unique_ptr<smooth::application::sensor::AxpPMU> axp192{};
            bool axp192_initialized { false };
    };
}
```
Third, create M5StickC.cpp that uses the Axp192.
``` C++ code
M5StickC.cpp

#include "model/M5StickC.h"
#include "model/Axp192Init.h"

#include <smooth/core/logging/log.h>
#include <smooth/application/io/i2c/AxpRegisters.h>

using namespace smooth::core::logging;
using namespace smooth::core::ipc;
using namespace smooth::application::sensor;

namespace some_namespace_name
{
    // Class constants
    static const char* TAG = "M5StickC";

    // Constructor
    M5StickC::M5StickC() : i2c0_master(I2C_NUM_0,               // I2C Port 0
                                       GPIO_NUM_22,             // SCL pin
                                       true,                    // SCL internal pullup enabled
                                       GPIO_NUM_21,             // SDA pin
                                       true,                    // SDA internal pullup enabled
                                       400 * 1000)              // clock frequency - 400kHz
    {
    }

    // Initialize the M5StickC
    void M5StickC::initialize()
    {
        initialize_axp192();
    }

    // Set screen brightness - LDO2 voltage is used to adjust screen brightness
    void M5StickC::set_screen_brightness(uint8_t brightness)
    {
        // max brightness level is 12 = 3.00V
        brightness = brightness > 12 ? 12 : brightness;
        axp192->write_register_bits(AxpRegister::Reg28H_Ldo2_Ldo3_VSet, 0x0F, 4, brightness);
    }

    // Initialize the AXP192
    void M5StickC::initialize_axp192()
    {
        auto device = i2c0_master.create_device<AxpPMU>(0x34);   // AXP192 i2c device address = 0x34
        Log::info(TAG, "Scanning for Axp192 ---- {}", device->is_present() ? "device found" : "device NOT present");

        if (device->is_present())
        {
            Log::info(TAG, "Configuring AXP192");
            axp192_initialized = device->write_init_regs(axp192_init_cmds_1.data(), axp192_init_cmds_1.size());

            if (axp192_initialized)
            {
                axp192 = std::move(device);
            }
        }

        Log::info(TAG, "Axp192 initialization --- {}", axp192_initialized ? "Succeeded" : "Failed");
    }

    // Print Ax192 measurements
    void M5StickC::print_axp192_report()
    {
        float value;
        Log::error(TAG, "===============================================");

        axp192->get_ts_voltage(value);
        Log::warning(TAG, "TS voltage       = {:.2f} V", value);

        axp192->get_acin_voltage(value);
        Log::warning(TAG, "ACIN voltage     = {:.2f} V", value);

        axp192->get_acin_current(value);
        Log::warning(TAG, "ACIN current     = {:.2f} mA", value);

        axp192->get_vbus_voltage(value);
        Log::warning(TAG, "VBUS voltage     = {:.2f} V", value);

        axp192->get_vbus_current(value);
        Log::warning(TAG, "VBUS current     = {:.2f} mA", value);

        axp192->get_battery_voltage(value);
        Log::warning(TAG, "BATT voltage     = {:.2f} V", value);

        axp192->get_aps_voltage(value);
        Log::warning(TAG, "APS voltage      = {:.2f} V", value);

        axp192->get_axp_device_temperature(value);
        Log::warning(TAG, "Device temperature         = {:.2f} C", value);

        axp192->get_battery_charging_current(value);
        Log::warning(TAG, "BATT charging current      = {:.2f} mA", value);

        axp192->get_battery_discharging_current(value);
        Log::warning(TAG, "BATT discharging current   = {:.2f} mA", value);

        axp192->get_battery_power(value);
        Log::warning(TAG, "BATT power                 = {:.2f} mW", value);

        axp192->get_battery_capacity(value);
        Log::warning(TAG, "BATT capacity              = {:.2f} mAh", value);
    }
}
```

