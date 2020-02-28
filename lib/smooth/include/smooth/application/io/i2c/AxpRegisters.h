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
#pragma once

#include <cstdint>
#include <stdint.h>

namespace smooth::application::sensor
{
    enum class AxpRegister : uint8_t
    {
        // Group 1 - Power Control
        Reg01H_Pwr_Chrg_Status = 0x00,
        Reg02H_Usb_Vbus_Status = 0x01,
        Reg12H_Power_Out_Ctrl = 0x12,
        Reg23H_Dcdc2_Volt_Set = 0x23,
        Reg25H_Dcdc2_Ldo3_Vrc = 0x25,
        Reg26H_Dcdc1_Volt_Set = 0x26,
        Reg27H_Dcdc3_Volt_Set = 0x27,
        Reg28H_Ldo2_Ldo3_VSet = 0x28,
        Reg29H_Ldo3_Volt_Sett = 0x29,
        Reg30H_Ips_Out_Mngmnt = 0x30,
        Reg31H_Voff_Volt_Sett = 0x31,
        Reg32H_Shutdn_ChrgLed = 0x32,
        Reg33H_Chrg_Control_1 = 0x33,
        Reg34H_Chrg_Control_2 = 0x34,
        Reg35H_Backup_Chrg_Ctl = 0x35,
        Reg36H_PEK_Key_Setting = 0x36,
        Reg37H_Dcdc_Freq_Set = 0x37,
        Reg38H_Vlth_Chrg_Set = 0x38,
        Reg39H_Vhth_Chrg_Set = 0x39,
        Reg3AH_APS_VWarning1 = 0x3A,
        Reg3BH_APS_VWarning2 = 0x3B,
        Reg3CH_Vlth_DisChrg_Set = 0x3C,
        Reg3DH_Vhth_DisChrg_Set = 0x3D,
        Reg80H_Dcdc_Mode_Set = 0x80,
        Reg82H_Adc_Enable_1 = 0x82,
        Reg83H_Adc_Enable_2 = 0x83,
        Reg84H_Adc_Sample_Rate = 0x84,
        Reg85H_Adc_Input_Range = 0x85,
        Reg86H_Adc_Irq_RE_Thld = 0x86,
        Reg87H_Adc_Irq_FE_Thld = 0x87,
        Reg8AH_Timer_Control = 0x8A,
        Reg8BH_Vbus_Detect_Srp = 0x8B,
        Reg8FH_Ovr_Temp_Shutdn = 0x8F,

        // Group 2 - GPIO Control
        Reg90H_Gpio0_Func_Set = 0x90,
        Reg91H_Gpio0_Volt_Set = 0x91,
        Reg92H_Gpio1_Func_Set = 0x92,
        Reg93H_Gpio2_Func_Set = 0x93,
        Reg94H_Gpio_012_Status = 0x94,
        Reg95H_Gpio_34_Fnc_Set = 0x95,
        Reg96H_Gpio_34_Status = 0x96,
        Reg97H_Gpio_012_PD_Ctl = 0x97,
        Reg98H_Pwm1_Freq_Set = 0x98,
        Reg99H_Pwm1_Duty_Set1 = 0x99,
        Reg9AH_Pwm1_Duty_Set2 = 0x9A,
        Reg9BH_Pwm2_Freq_Set = 0x9B,
        Reg9CH_Pwm2_Duty_Set1 = 0x9C,
        Reg9DH_Pwm2_Duty_Set2 = 0x9D,
        Reg9EH_NRSTO_Func_Set = 0x9E,

        // Group 3 - Interrupt Control
        Reg40H_Irq_Enable_Grp1 = 0x40,
        Reg41H_Irq_Enable_Grp2 = 0x41,
        Reg42H_Irq_Enable_Grp3 = 0x42,
        Reg43H_Irq_Enable_Grp4 = 0x43,
        Reg4AH_Irq_Enable_Grp5 = 0x4A,
        Reg44H_Irq_Status_Grp1 = 0x44,
        Reg45H_Irq_Status_Grp2 = 0x45,
        Reg46H_Irq_Status_Grp3 = 0x46,
        Reg47H_Irq_Status_Grp4 = 0x47,
        Reg4DH_Irq_Status_Grp5 = 0x4D,

        // Group 4 - ADC Values
        Reg56H_Acin_Volt_HI8 = 0x56,
        Reg57H_Acin_Volt_LO4 = 0x57,
        Reg58H_Acin_Curr_HI8 = 0x58,
        Reg59H_Acin_Curr_LO4 = 0x59,
        Reg5AH_Vbus_Volt_HI8 = 0x5A,
        Reg5BH_Vbus_Volt_LO4 = 0x5B,
        Reg5CH_Vbus_Curr_HI8 = 0x5C,
        Reg5DH_Vbus_Curr_LO4 = 0x5D,
        Reg5EH_Intr_Temp_HI8 = 0x5E,
        Reg5FH_Intr_Temp_LO4 = 0x5F,
        Reg62H_Batt_Temp_HI8 = 0x62,
        Reg63H_Batt_Temp_LO4 = 0x63,
        Reg64H_Gpio0_Volt_HI8 = 0x64,
        Reg65H_Gpio0_Volt_LO4 = 0x65,
        Reg66H_Gpio1_Volt_HI8 = 0x66,
        Reg67H_Gpio1_Volt_LO4 = 0x67,
        Reg68H_Gpio2_Volt_HI8 = 0x68,
        Reg69H_Gpio2_Volt_LO4 = 0x69,
        Reg6AH_Gpio3_Volt_HI8 = 0x6A,
        Reg6BH_Gpio3_Volt_LO4 = 0x6B,
        Reg70H_Batt_Power_HI8 = 0x70,
        Reg71H_Batt_Power_MI8 = 0x71,
        Reg72H_Batt_Power_LO8 = 0x72,
        Reg78H_Batt_Volt_HI8 = 0x78,
        Reg79H_Batt_Volt_LO4 = 0x79,
        Reg7AH_Batt_Chrg_Curr_HI8 = 0x7A,
        Reg7BH_Batt_Chrg_Curr_LO5 = 0x7B,
        Reg7CH_Batt_DisChg_Curr_HI8 = 0x7C,
        Reg7DH_Batt_DisChg_Curr_LO5 = 0x7D,
        Reg7EH_Aps_Voltage_HI8 = 0x7E,
        Reg7FH_Aps_Voltage_LO4 = 0x7F,

        // Group 5 - Battery Coulomb Counter
        RegB0H_Batt_Chrg_Coulomb3 = 0xB0,
        RegB1H_Batt_Chrg_Coulomb2 = 0xB1,
        RegB2H_Batt_Chrg_Coulomb1 = 0xB2,
        RegB3H_Batt_Chrg_Coulomb0 = 0xB3,
        RegB4H_Batt_DisChg_Coulomb3 = 0xB4,
        RegB5H_Batt_DisChg_Coulomb2 = 0xB5,
        RegB6H_Batt_DisChg_Coulomb1 = 0xB6,
        RegB7H_Batt_DisChg_Coulomb0 = 0xB7,
        RegB8H_Coulomb_Counter_Ctrl = 0xB8,
        RegB9_Batt_Fuel_Guage = 0xB9
    };

    struct AxpInitReg
    {
        AxpRegister reg;
        uint8_t data;
    };
}
