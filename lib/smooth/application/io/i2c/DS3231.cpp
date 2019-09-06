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

#include <iostream>
#include <vector>
#include <sstream>
#include <date/date.h>
#include <smooth/core/util/ByteSet.h>
#include <smooth/core/logging/log.h>
#include <smooth/application/io/i2c/DS3231.h>

namespace smooth
{
    namespace application
    {
        namespace io
        {
            using namespace std::chrono;
            using namespace date;

            DS3231::DS3231(i2c_port_t port, uint8_t address, std::mutex& guard)
                    : I2CMasterDevice(port, address, guard)
            {
            }

            uint8_t DS3231::BCD2Uint8_t(uint8_t value)
            {
                return static_cast<uint8_t>(((value >> 4) * 10) + (value & 0x0f));
            }

            uint8_t DS3231::Uint8_t2BCD(uint8_t value)
            {
                return static_cast<uint8_t>(((value / 10) << 4) + (value % 10));
            }

            system_clock::time_point DS3231::GetDateTime()
            {
                core::util::FixedBuffer<uint8_t, 7> data;
                auto res = read(address, Register::DateTime, data);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the the Control register.");

                    return system_clock::from_time_t(0);
                }

                auto date = year(2000 + (int)BCD2Uint8_t(data[6]) + ((data[5] & 0x80) ? 100 : 0))
                            / (BCD2Uint8_t(data[5] & 0x7f) - 1) / BCD2Uint8_t(data[4]);

                return sys_days(date)
                       + hours(
                    (data[2] & 0x40)
                        ? BCD2Uint8_t(data[2] & 0x1F) - 1 + (data[2] & 0x20) ? 12 : 0 /* 12H */
                        : BCD2Uint8_t(data[2] & 0x3F) /* 24H */
                    )
                       + minutes(BCD2Uint8_t(data[1]))
                       + seconds(BCD2Uint8_t(data[0]));
            }

            bool DS3231::SetDateTime(system_clock::time_point& dateTime)
            {
                // TODO check that dateTime is valid
                auto timePointWithDaysResolution = floor<days>(dateTime);
                auto date = year_month_day(timePointWithDaysResolution);
                auto time = make_time(dateTime - timePointWithDaysResolution);

                std::vector<uint8_t> data;
                data.push_back(Register::DateTime);
                data.push_back(Uint8_t2BCD(static_cast<uint8_t>(time.seconds().count())));
                data.push_back(Uint8_t2BCD(static_cast<uint8_t>(time.minutes().count())));
                data.push_back(Uint8_t2BCD(static_cast<uint8_t>(time.hours().count())));
                data.push_back(Uint8_t2BCD(static_cast<uint8_t>((year_month_weekday(date).weekday_indexed().weekday()
                                                                 - Sunday).count())));
                data.push_back(Uint8_t2BCD( static_cast<uint8_t>(unsigned(date.day()))));
                uint8_t year = static_cast<uint8_t>((int)date.year() - 2000);
                data.push_back(Uint8_t2BCD(
                    static_cast<uint8_t>((unsigned(date.month()) + 1) | (year >= 100 ? 0x80 : 0))));
                data.push_back(Uint8_t2BCD(static_cast<uint8_t>(year - (year >= 100 ? 100 : 0))));

                return write(address, data, true);
            }

            bool DS3231::SetAlarmOne(AlarmOneRate rate, system_clock::time_point const& timePoint)
            {
                // TODO check that dateTime is valid
                auto timePointWithDaysResolution = floor<days>(timePoint);
                auto date = year_month_day(timePointWithDaysResolution);
                auto time = make_time(timePoint - timePointWithDaysResolution);

                std::vector<uint8_t> data;
                data.push_back(Register::Alarm1);
                data.push_back(rate >= SecondsMatch ? Uint8_t2BCD(static_cast<uint8_t>(time.seconds().count())) : 0x80);
                data.push_back(rate >= MinutesAndSecondsMatch ? Uint8_t2BCD(
                    static_cast<uint8_t>(time.minutes().count())) : 0x80);
                data.push_back(rate >=
                HoursMinutesAndSecondsMatch ? Uint8_t2BCD(static_cast<uint8_t>(time.hours().count())) : 0x80);
                data.push_back(
                    rate >= DateHoursMinutesAndSecondsMatch
                    ? Uint8_t2BCD(static_cast<uint8_t>(unsigned(date.day())))
                    : (rate ==
                       DayHoursMinutesAndSecondsMatch ? Uint8_t2BCD(static_cast<uint8_t>((weekday{ date }
                                                                                          - Sunday).count()))
                       & 0x40 : 0x80)
                    );

                return write(address, data, true);
            }

            bool DS3231::SetAlarmTwo(AlarmTwoRate rate, system_clock::time_point const& timePoint)
            {
                // TODO check that dateTime is valid
                auto timePointWithDaysResolution = floor<days>(timePoint);
                auto date = year_month_day(timePointWithDaysResolution);
                auto time = make_time(timePoint - timePointWithDaysResolution);

                std::vector<uint8_t> data;
                data.push_back(Register::Alarm2);
                data.push_back(rate >= MinutesMatch ? Uint8_t2BCD(static_cast<uint8_t>(time.minutes().count())) : 0x80);
                data.push_back(rate >= HoursAndMinutesMatch ? Uint8_t2BCD(
                    static_cast<uint8_t>(time.hours().count())) : 0x80);
                data.push_back(
                    rate >= DateHoursAndMinutesMatch
                    ? Uint8_t2BCD(static_cast<uint8_t>(unsigned(date.day())))
                    : (rate ==
                       DayHoursAndMinutesMatch ? Uint8_t2BCD(static_cast<uint8_t>((weekday{ date } - Sunday).count()))
                       & 0x40 : 0x80)
                    );

                return write(address, data, true);
            }

            bool DS3231::IsOscillatorEnabled()
            {
                core::util::FixedBuffer<uint8_t, 1> data;
                auto res = read(address, Register::Control, data);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the the Control register.");

                    return false;
                }

                return !(data[0] & 0x80);
            }

            bool DS3231::EnableOscillator(bool enable)
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                std::vector<uint8_t> data;
                data.push_back(Register::Control);

                if (enable)
                {
                    data.push_back(controlRegister[0] & uint8_t(~0x80));
                }
                else
                {
                    data.push_back(controlRegister[0] | 0x80);
                }

                return write(address, data, true);
            }

            bool DS3231::IsBatteryBackedSquareWaveEnabled()
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                return controlRegister[0] & 0x40;
            }

            bool DS3231::EnableBatteryBackedSquareWave(bool enable)
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                std::vector<uint8_t> data;
                data.push_back(Register::Control);

                if (enable)
                {
                    data.push_back(controlRegister[0] | 0x80);
                }
                else
                {
                    data.push_back(controlRegister[0] & uint8_t(~0x80));
                }

                return write(address, data, true);
            }

            bool DS3231::IsConvertingTemperature()
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                return controlRegister[0] & 0x20;
            }

            bool DS3231::ConvertTemperature()
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                auto isConvertingTemp = controlRegister[0] & 0x20;

                if (!isConvertingTemp)
                {
                    std::vector<uint8_t> data;
                    data.push_back(Register::Control);
                    data.push_back(controlRegister[0] | 0x20);
                    auto result = write(address, data, true);
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));

                    return result;
                }
                else
                {
                    Log::warning("DS3231", "A user initiated termperature conversion is already active.");

                    return false;
                }
            }

            DS3231::SquareWaveOutputFrequency DS3231::GetSquareWaveOutputFrequency()
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return Frequency_8kHz;
                }

                return SquareWaveOutputFrequency(controlRegister[0] & Frequency_8kHz);
            }

            bool DS3231::SetSquareWaveOutputFrequency(SquareWaveOutputFrequency frequency)
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                auto d = controlRegister[0] & ~0x04;
                d |= (frequency & Frequency_8kHz);

                std::vector<uint8_t> data;
                data.push_back(Register::Control);
                data.push_back((uint8_t)d);

                return write(address, data, true);
            }

            bool DS3231::IsInterruptControlEnabled()
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                return controlRegister[0] & 0x04;
            }

            bool DS3231::EnableInterruptControl(bool enable)
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                std::vector<uint8_t> data;
                data.push_back(Register::Control);

                if (enable)
                {
                    data.push_back(controlRegister[0] | 0x04);
                }
                else
                {
                    data.push_back(controlRegister[0] & uint8_t(~0x04));
                }

                return write(address, data, true);
            }

            bool DS3231::IsAlarm1InterruptEnabled()
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                return controlRegister[0] & 0x01;
            }

            bool DS3231::EnableAlarm1Interrupt(bool enable)
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                std::vector<uint8_t> data;
                data.push_back(Register::Control);

                if (enable)
                {
                    data.push_back(controlRegister[0] | 0x01);
                }
                else
                {
                    data.push_back(controlRegister[0] & uint8_t(~0x01));
                }

                return write(address, data, true);
            }

            bool DS3231::IsAlarm2InterruptEnabled()
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                return controlRegister[0] & 0x02;
            }

            bool DS3231::EnableAlarm2Interrupt(bool enable)
            {
                core::util::FixedBuffer<uint8_t, 1> controlRegister;
                auto res = read(address, Register::Control, controlRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Control register.");

                    return false;
                }

                std::vector<uint8_t> data;
                data.push_back(Register::Control);

                if (enable)
                {
                    data.push_back(controlRegister[0] | 0x02);
                }
                else
                {
                    data.push_back(controlRegister[0] & uint8_t(~0x02));
                }

                return write(address, data, true);
            }

            bool DS3231::IsOscillatorStopped()
            {
                core::util::FixedBuffer<uint8_t, 1> data;
                auto res = read(address, Register::Status, data);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Status register.");

                    return false;
                }

                auto oscillatorStopBit = data[0] & 0x80;

                if (oscillatorStopBit)
                {
                    Log::info("DS3231", "The oscillator was stopped, possible causes are:");
                    Log::info("DS3231", "\tPower has been applied for the first time.");
                    Log::info("DS3231", "\tThe VCC and VBAT voltages are insufficient for oscillation.");

                    if (!IsOscillatorEnabled() & IsBatteryBackedSquareWaveEnabled())
                    {
                        Log::info("DS3231", "\tThe EOSC bit is turned off in battery-backed mode.");
                    }

                    Log::info("DS3231", "\tExternal influences on the crystal (i.e., noise, leakage, etc.).");
                }

                return oscillatorStopBit;
            }

            bool DS3231::ClearOscillatorStoppedFlag()
            {
                core::util::FixedBuffer<uint8_t, 1> statusRegister;
                auto res = read(address, Register::Status, statusRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Status register.");

                    return false;
                }

                std::vector<uint8_t> data;
                data.push_back(Register::Status);
                data.push_back(statusRegister[0] & uint8_t(~0x80));

                return write(address, data, true);
            }

            bool DS3231::Is32kHzOutputEnabled()
            {
                core::util::FixedBuffer<uint8_t, 1> data;
                auto res = read(address, Register::Status, data);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Status register.");

                    return false;
                }

                return data[0] & 0x08;
            }

            bool DS3231::Enable32kHzOutput(bool enable)
            {
                core::util::FixedBuffer<uint8_t, 1> statusRegister;
                auto res = read(address, Register::Status, statusRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Status register.");

                    return false;
                }

                std::vector<uint8_t> data;
                data.push_back(Register::Status);

                if (enable)
                {
                    data.push_back(statusRegister[0] | 0x08);
                }
                else
                {
                    data.push_back(statusRegister[0] & uint8_t(~0x08));
                }

                return write(address, data, true);
            }

            bool DS3231::IsBusy()
            {
                core::util::FixedBuffer<uint8_t, 1> data;
                auto res = read(address, Register::Status, data);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Status register.");

                    return false;
                }

                return data[0] & 0x04;
            }

            bool DS3231::IsAlarm1Triggered()
            {
                core::util::FixedBuffer<uint8_t, 1> data;
                auto res = read(address, Register::Status, data);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Status register.");

                    return false;
                }

                return data[0] & 0x01 && IsAlarm1InterruptEnabled();
            }

            bool DS3231::ClearAlarm1Flag()
            {
                core::util::FixedBuffer<uint8_t, 1> statusRegister;
                auto res = read(address, Register::Status, statusRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Status register.");

                    return false;
                }

                std::vector<uint8_t> data;
                data.push_back(Register::Status);
                data.push_back(statusRegister[0] & uint8_t(~0x01));

                return write(address, data, true);
            }

            bool DS3231::IsAlarm2Triggered()
            {
                core::util::FixedBuffer<uint8_t, 1> data;
                auto res = read(address, Register::Status, data);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Status register.");

                    return false;
                }

                return data[0] & 0x02 && IsAlarm2InterruptEnabled();
            }

            bool DS3231::ClearAlarm2Flag()
            {
                core::util::FixedBuffer<uint8_t, 1> statusRegister;
                auto res = read(address, Register::Status, statusRegister);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the Status register.");

                    return false;
                }

                std::vector<uint8_t> data;
                data.push_back(Register::Status);
                data.push_back(statusRegister[0] & uint8_t(~0x02));

                return write(address, data, true);
            }

            int8_t DS3231::GetAgingOffset()
            {
                core::util::FixedBuffer<uint8_t, 1> data;
                auto res = read(address, Register::AgingOffset, data);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the the Temperature register.");

                    return false;
                }

                return (int8_t)data[0];
            }

            bool DS3231::SetAgingOffset(int8_t offset)
            {
                std::vector<uint8_t> data;
                data.push_back(Register::AgingOffset);
                data.push_back((uint8_t)offset);
                auto result = write(address, data, true);
                ConvertTemperature();

                return result;
            }

            float DS3231::GetTemperature()
            {
                core::util::FixedBuffer<uint8_t, 2> data;
                auto res = read(address, Register::Temperature, data);

                if (!res)
                {
                    Log::error("DS3231", "Unable to read the the Temperature register.");

                    return false;
                }

                return (float)(data[0] << 2 | data[1] >> 6) * (float)0.25;
            }
        }
    }
}
