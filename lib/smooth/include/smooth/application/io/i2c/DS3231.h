// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.

// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <chrono>
#include <smooth/core/Task.h>
#include <smooth/core/io/InterruptInput.h>
#include <smooth/core/io/i2c/I2CMasterDevice.h>
#include <smooth/core/util/FixedBuffer.h>
#include <memory>
#include <utility>

namespace smooth::application::io
{
    using namespace std::chrono;

    class DS3231
        : public core::io::i2c::I2CMasterDevice
    {
        private:
            uint8_t BCD2Uint8_t(uint8_t value);

            uint8_t Uint8_t2BCD(uint8_t value);

        public:
            DS3231(i2c_port_t port, uint8_t address, std::mutex& guard);

            enum Register
            {
                DateTime = 0x00,
                Alarm1 = 0x07,
                Alarm2 = 0x0b,
                Control = 0x0e,
                Status = 0x0f,
                AgingOffset = 0x10,
                Temperature = 0x11
            };

            enum SquareWaveOutputFrequency
            {
                Frequency_1Hz = 0x00,
                Frequency_1kHz = 0x08,
                Frequency_4kHz = 0x10,
                Frequency_8kHz = 0x18
            };

            enum AlarmOneRate
            {
                OncePerSecond = 0,
                SecondsMatch,
                MinutesAndSecondsMatch,
                HoursMinutesAndSecondsMatch,
                DateHoursMinutesAndSecondsMatch,
                DayHoursMinutesAndSecondsMatch
            };

            enum AlarmTwoRate
            {
                OncePerMinute = 0,
                MinutesMatch,
                HoursAndMinutesMatch,
                DateHoursAndMinutesMatch,
                DayHoursAndMinutesMatch
            };

            //void event(const smooth::core::io::InterruptInputEvent& value) override;

            /// Gets the date and time
            /// \return a time_point,
            ///		minimum time_point if a failure reading the date and time occurred
            system_clock::time_point GetDateTime();

            /// Sets the date and time
            /// \param dateTime The date and time
            /// \return true if the date and time was set,
            ///		false if there was a failure writing the bit
            bool SetDateTime(system_clock::time_point& dateTime);

            //AlarmOne GetAlarmOne();

            /// Sets alarm one
            /// \param timePoint The date and time of the alarm
            /// \param rate The rate of the alarm
            /// \return true if the alarm was set,
            ///		false if there was a failure writing the alarm data
            bool SetAlarmOne(AlarmOneRate rate = OncePerSecond,
                             system_clock::time_point const& timePoint = system_clock::from_time_t(0));

            //AlarmTwo GetAlarmTwo();

            /// Sets alarm two
            /// \param timePoint The date and time of the alarm
            /// \param rate The rate of the alarm
            /// \return true if the alarm was set,
            ///		false if there was a failure writing the alarm data
            bool SetAlarmTwo(AlarmTwoRate rate = OncePerMinute,
                             system_clock::time_point const& timePoint = system_clock::from_time_t(0));

            /// Reads the Enable Oscillator (EOSC) bit
            /// \return true if the oscillator is enabled,
            ///		false if the oscillator is disabled or a failure reading the bit occurred
            bool IsOscillatorEnabled();

            /// Enables/Disables the Oscillator (EOSC) bit
            /// \param enable Set to true to enable or false to disable
            /// \return true if the Oscillator was enabled/disabled,
            ///		false if there was a failure writing the bit
            ///
            /// When set to logic 0, the oscillator is started.
            /// When set to logic 1, the oscillator is stopped when the DS3231 switches to VBAT.
            /// This bit is clear(logic 0) when power is first applied.
            /// When the DS3231 is powered by VCC, the oscillator is always on
            ///	regardless of the status of the EOSC bit.
            /// When EOSC is disabled, all register data is static
            bool EnableOscillator(bool enable);

            /// Reads the Battery-Backed Square-Wave Enable (BBSQW) bit
            /// \return true if the oscillator has been stopped,
            ///		false if the oscillator is running or a failure reading the flag occurred
            bool IsBatteryBackedSquareWaveEnabled();

            /// Enables/Disables the Battery-Backed Square-Wave (BBSQW) bit
            /// \param enable Set to true to enable or false to disable
            /// \return true if Battery-Backed Square-Wave was enabled/disabled,
            ///		false if there was a failure writing the bit
            ///
            /// When set to logic 1 with INTCN = 0 and VCC < VPF, this bit enables the square wave.
            /// When BBSQW is logic 0, the INT/SQW pin goes high impedance when VCC < VPF.
            ///	This bit is disabled(logic 0) when power is first applied.
            bool EnableBatteryBackedSquareWave(bool enable);

            /// Reads the Convert Temperature flag (CONV)
            /// \return true if Convert Temperature is busy,
            ///		false if not busy or a failure reading the flag occurred
            bool IsConvertingTemperature();

            /// Forces the temperature sensor to convert the temperature into digital code,
            /// and execute the TCXO algorithm to update the capacitance array to the oscillator.
            /// \return true if the 32kHz Output was enabled,
            ///		false if failure writing the bit
            ///
            /// Setting this bit to 1 forces the temperature sensor to convert the temperature
            /// into digital code and execute the TCXO algorithm to update the capacitance array to the oscillator.
            /// This can only happen when a conversion is not already in progress.
            /// The user should check the status bit BSY before forcing the controller to start a new TCXO execution.
            /// A user - initiated temperature conversion does not affect the internal 64 - second update cycle.
            ///	A user - initiated temperature conversion does not affect the BSY bit for approximately 2ms.
            /// The CONV bit remains at a 1 from the time it is written until the conversion is finished,
            /// at which time both CONV and BSY go to 0.
            /// The CONV bit should be used when monitoring the status of a user - initiated conversion.
            bool ConvertTemperature();

            /// Reads the Rate Select bits (RS2 and RS1)
            /// \return the Square Wave Output Frequency,
            ///		Frequency_8kHz if a failure reading the bits occurred
            SquareWaveOutputFrequency GetSquareWaveOutputFrequency();

            /// Sets the Square-Wave Output Frequemcy (RS2 and RS1) bits
            /// \param frequency Set the square-wave output frequency
            /// \return true if the Square-Wave Output Frequency was set,
            ///		false if there was a failure writing the bits
            /// These bits control the frequency of the square - wave output when the square wave has been enabled.
            /// The following table shows the square - wave frequencies that can be selected with the RS bits.
            /// These bits are both set to logic 1 (8.192kHz) when power is first applied.
            bool SetSquareWaveOutputFrequency(SquareWaveOutputFrequency frequency);

            /// Reads the Interrupt Control Enabled flag (INTCN)
            /// \return true if Interrupt Control is enabled,
            ///		false if not enabled or a failure reading the flag occurred
            bool IsInterruptControlEnabled();

            /// Enables/Disables the Interrupt Control (INTCN) bit
            /// \param enable True to enable or false to disable
            /// \return true if the Interrupt Control was enabled/disabled,
            ///		false if there was a failure writing the bit
            /// This bit controls the INT / SQW signal.
            /// When the INTCN bit is set to logic 0, a square wave is output on the INT / SQW pin.
            /// When the INTCN bit is set to logic 1, then a match between the timekeeping registers
            /// and either of the alarm registers activates the INT / SQW output(if the alarm is also enabled).
            /// The corresponding alarm flag is always set regardless of the state of the INTCN bit.
            /// The INTCN bit is set to logic 1 when power is first applied.
            bool EnableInterruptControl(bool enable);

            /// Reads the Alarm 1 Interrupt Enabled flag (A1IE)
            /// \return true if Alarm 1 Interrupt is enabled,
            ///		false if not enabled or a failure reading the flag occurred
            bool IsAlarm1InterruptEnabled();

            /// Enables/Disables the Alarm 1 Interrupt (A1IE) bit
            /// \param enable True to enable or false to disable
            /// \return true if the Alarm 1 Interrupt was enabled/disabled,
            ///		false if there was a failure writing the bit
            ///
            /// When set to logic 1, this bit permits the alarm 1 flag(A1F) bit in the
            /// status register to assert INT / SQW(when INTCN = 1).
            ///	When the A1IE bit is set to logic 0 or INTCN is set to logic
            ///	0, the A1F bit does not initiate the INT / SQW signal.
            /// The A1IE bit is disabled(logic 0) when power is first applied.
            bool EnableAlarm1Interrupt(bool enable);

            /// Reads the Alarm 2 Interrupt Enabled flag (A2IE)
            /// \return true if Alarm 2 Interrupt is enabled,
            ///		false if not enabled or a failure reading the flag occurred
            bool IsAlarm2InterruptEnabled();

            /// Enables/Disables the Alarm 1 Interrupt (A1IE) bit
            /// \param enable True to enable or false to disable
            /// \return true if the Alarm 1 Interrupt was enabled/disabled,
            ///		false if there was a failure writing the bit
            ///
            /// When set to logic 1, this bit permits the alarm 1 flag(A1F) bit in the
            /// status register to assert INT / SQW(when INTCN = 1).
            ///	When the A1IE bit is set to logic 0 or INTCN is set to logic
            ///	0, the A1F bit does not initiate the INT / SQW signal.
            /// The A1IE bit is disabled(logic 0) when power is first applied.
            bool EnableAlarm2Interrupt(bool enable);

            /// Reads the Oscillator Stop flag (OSF)
            /// \return true if the oscillator has been stopped,
            ///		false if the oscillator is running or a failure reading the flag occurred
            ///
            /// A logic 1 in this bit indicates that the oscillator either is stopped or was stopped
            /// for some periodand may be used to judge the validity of
            ///	the timekeeping data.This bit is set to logic 1 any time
            ///	that the oscillator stops.The following are examples of
            ///	conditions that can cause the OSF bit to be set :
            /// 1) The first time power is applied.
            /// 2) The voltages present on both VCCand VBAT are insufficient to support oscillation.
            /// 3) The EOSC bit is turned off in battery - backed mode.
            /// 4) External influences on the crystal(i.e., noise, leakage,
            /// etc.).
            ///	This bit remains at logic 1 until written to logic 0
            bool IsOscillatorStopped();

            /// Clears the Oscillator Stop flag (OSF)
            /// \return false if the oscillator could not be cleared
            bool ClearOscillatorStoppedFlag();

            /// Reads the Enable 32kHz Output flag (EN32kHz)
            /// \return true if 32kHz Output is enabled,
            ///		false if 32kHz Output is disabled or a failure reading the flag occurred
            bool Is32kHzOutputEnabled();

            /// Enables/Disables the 32kHz Output (EN32kHz) bit
            /// \param enable True to enable or false to disable
            /// \return true if 32kHz Output was enabled/disabled,
            ///		false if there was a failure writing the bit
            ///
            /// This bit controls the status of the 32kHz pin.
            /// When set to logic 1, the 32kHz pin is enabled and outputs a 32.768kHz squarewave signal.
            /// When set to logic 0, the 32kHz pin goes to a high - impedance state.
            /// The initial power - up state of this bit is logic 1, and a 32.768kHz square - wave signal appears
            ///	at the 32kHz pin after a power source is applied to the DS3231 (if the oscillator is running).
            bool Enable32kHzOutput(bool enable);

            /// Reads the Busy flag (BSY)
            /// \return true if busy,
            ///		false if not busy or a failure reading the flag occurred
            ///
            /// This bit indicates the device is busy executing TCXO functions.
            /// It goes to logic 1 when the conversion signal to the temperature sensor is asserted and
            ///	then is cleared when the device is in the 1 - minute idle state.
            bool IsBusy();

            /// Reads the Alarm 1 flag (A1F)
            /// \return true if Alarm 1 triggered,
            ///		false if not busy or a failure reading the flag occurred
            ///
            /// A logic 1 in the alarm 1 flag bit indicates that the time matched the alarm 1 registers.
            /// If the A1IE bit is logic 1 and the INTCN bit is set to logic 1, the INT/SQW pin is also asserted.
            /// A1F is cleared when written to logic 0. This bit can only be written to logic 0. Attempting
            ///	to write to logic 1 leaves the value unchanged.
            bool IsAlarm1Triggered();

            /// Clears the Alarm 1 flag (A1F)
            /// \return true if Alarm 1 flag was cleared,
            ///		false if there was a failure writing the bit
            bool ClearAlarm1Flag();

            /// Reads the Alarm 2 flag (A2F)
            /// \return true if Alarm 2 triggered,
            ///		false if not busy or a failure reading the flag occurred
            ///
            /// A logic 1 in the alarm 2 flag bit indicates that the time matched the alarm 2 registers.
            /// If the A2IE bit is logic 1 and the INTCN bit is set to logic 1, the INT/SQW pin is also asserted.
            /// A2F is cleared when written to logic 0. This bit can only be written to logic 0. Attempting
            ///	to write to logic 1 leaves the value unchanged.
            bool IsAlarm2Triggered();

            /// Clears the Alarm 2 flag (A1F)
            /// \return true if Alarm 2 flag was cleared,
            ///		false if there was a failure writing the bit
            bool ClearAlarm2Flag();

            /// Reads the Aging Offset register
            /// \return the offset
            int8_t GetAgingOffset();

            /// Sets the Aging Offset register
            /// \param offset The aging offset
            /// \return true if the Aging Offset register was set,
            ///		false if there was a failure writing the register
            ///
            /// The aging offset register takes a user-provided value to
            /// add to or subtract from the codes in the capacitance array
            ///	registers. The code is encoded in two’s complement, with
            ///	bit 7 representing the sign bit. One LSB represents one
            ///	small capacitor to be switched in or out of the capacitance
            ///	array at the crystal pins.
            /// The aging offset register capacitance value is added or subtracted from the capacitance
            ///	value that the device calculates for each temperature compensation.
            /// The offset register is added to the capacitance array during a normal temperature conversion, if
            ///	the temperature changes from the previous conversion, or
            ///	during a manual user conversion(setting the CONV bit).
            ///	To see the effects of the aging register on the 32kHz output frequency immediately, a manual conversion
            // should
            ///	be started after each aging register change.
            ///	Positive aging values add capacitance to the array, slowing the oscillator frequency.
            /// Negative values remove capacitance from the array, increasing the oscillator frequency.
            ///	The change in ppm per LSB is different at different temperatures.
            /// The frequency vs. temperature curve is shifted by the values used in this register.
            /// At + 25°C, one LSB typically provides about 0.1ppm change in frequency.
            ///	Use of the aging register is not needed to achieve the
            ///	accuracy as defined in the EC tables, but could be used
            ///	to help compensate for aging at a given temperature.
            ///	See the Typical Operating Characteristics section for a
            ///	graph showing the effect of the register on accuracy over
            ///	temperature.
            bool SetAgingOffset(int8_t offset);

            /// Reads the temperature register (A2F)
            /// \return true if Alarm 2 triggered,
            ///		-41 if a failure reading the flag occurred
            float GetTemperature();
    };
}
