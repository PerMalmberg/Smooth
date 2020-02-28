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
#include <vector>
#include "smooth/application/io/i2c/PCF8563.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core;
using namespace smooth::core::logging;

namespace smooth::application::sensor
{
    // Class constants
    static const char* TAG = "PCF8563";

    // Constructor
    PCF8563::PCF8563(i2c_port_t port, uint8_t address, std::mutex& guard)
            : I2CMasterDevice(port, address, guard)
    {
    }

    // Get time
    bool PCF8563::get_rtc_time(RtcTime& rtc_time)
    {
        util::FixedBuffer<uint8_t, 7> rd_data;
        bool res = read(address, static_cast<uint8_t>(Rtc8563Register::Seconds), rd_data);
        bool is_time_valid = !((rd_data[0] & 0x80) >> 7);

        if (res & is_time_valid)
        {
            rtc_time.seconds = bcd_to_decimal(rd_data[0] & 0x7F);
            rtc_time.minutes = bcd_to_decimal(rd_data[1] & 0x7F);
            rtc_time.hours24 = bcd_to_decimal(rd_data[2] & 0x3F);
            rtc_time.days = bcd_to_decimal(rd_data[3] & 0x3F);
            rtc_time.weekdays = static_cast<DayOfWeek>(bcd_to_decimal(rd_data[4] & 0x07));
            rtc_time.months = static_cast<Month>(bcd_to_decimal(rd_data[5] & 0x1F));
            rtc_time.years = static_cast<uint16_t>(2000 + bcd_to_decimal(rd_data[6]));
        }

        return res & is_time_valid;
    }

    // Set time
    bool PCF8563::set_rtc_time(RtcTime& rtc_time)
    {
        // validate rtc time values
        validate_time(rtc_time.seconds, "RTC seconds", 0, 59);
        validate_time(rtc_time.minutes, "RTC minutes", 0, 59);
        validate_time(rtc_time.hours24, "RTC hours", 0, 23);
        validate_year(rtc_time.years);
        validate_time(rtc_time.days, "RTC days", 1,
            number_of_days_in_month(rtc_time.months, rtc_time.years));

        // write rtc time values to chip
        std::vector<uint8_t> data{ static_cast<uint8_t>(Rtc8563Register::Seconds) };
        data.push_back(decimal_to_bcd(rtc_time.seconds));
        data.push_back(decimal_to_bcd(rtc_time.minutes));
        data.push_back(decimal_to_bcd(rtc_time.hours24));
        data.push_back(decimal_to_bcd(rtc_time.days));
        data.push_back(static_cast<uint8_t>(rtc_time.weekdays));
        data.push_back(decimal_to_bcd(static_cast<uint8_t>(rtc_time.months)));
        data.push_back(decimal_to_bcd(static_cast<uint8_t>(rtc_time.years - 2000)));

        return write(address, data);
    }

    // Convert BCD to decimal
    uint8_t PCF8563::bcd_to_decimal(uint8_t bcd)
    {
        return static_cast<uint8_t>((10 * ((bcd & 0xf0) >> 4)) + (bcd & 0xf));
    }

    // Convert decimal to BCD
    uint8_t PCF8563::decimal_to_bcd(uint8_t decimal)
    {
        return static_cast<uint8_t>(((decimal / 10) << 4) | (decimal % 10));
    }

    // The number of days in the month
    uint8_t PCF8563::number_of_days_in_month(Month month, uint16_t year)
    {
        uint8_t days = 31;

        if ((month == Month::April) | (month == Month::June) | (month == Month::September) | (month == Month::November))
        {
            days = 30;
        }

        if (month == Month::February)
        {
            // if leap year then days = 29 otherwise days = 28
            days = ((year % 4 == 0 && year % 100 != 0) | (year % 400 == 0)) ? 29 : 28;
        }

        return days;
    }

    // Get alarm time
    bool PCF8563::get_alarm_time(AlarmTime& alarm_time)
    {
        util::FixedBuffer<uint8_t, 4> rd_data;
        bool res = read(address, static_cast<uint8_t>(Rtc8563Register::MinuteAlarm), rd_data);

        if (res)
        {
            alarm_time.ena_alrm_minute = (rd_data[0] & 0x80) >> 7;
            alarm_time.ena_alrm_hour = (rd_data[1] & 0x80) >> 7;
            alarm_time.ena_alrm_day = (rd_data[2] & 0x80) >> 7;
            alarm_time.ena_alrm_weekday = (rd_data[3] & 0x80) >> 7;

            alarm_time.minute = bcd_to_decimal(rd_data[0] & 0x7F);
            alarm_time.hour24 = bcd_to_decimal(rd_data[1] & 0x3F);
            alarm_time.day = bcd_to_decimal(rd_data[2] & 0x3F);
            alarm_time.weekday = static_cast<DayOfWeek>(bcd_to_decimal(rd_data[3] & 0x07));
        }

        return res;
    }

    // Set Alarm time
    bool PCF8563::set_alarm_time(AlarmTime& alarm_time)
    {
        // Get current month and year from rtc chip
        util::FixedBuffer<uint8_t, 2> rd_data;
        bool res = read(address, static_cast<uint8_t>(Rtc8563Register::Months), rd_data);

        if (res)
        {
            // get month and year from chip
            Month month = static_cast<Month>(bcd_to_decimal(rd_data[0] & 0x1F));
            uint16_t year = static_cast<uint16_t>(2000 + bcd_to_decimal(rd_data[1]));

            // validate alarm time values
            validate_time(alarm_time.minute, "ALARM minute", 0, 59);
            validate_time(alarm_time.hour24, "ALARM hour", 0, 23);
            validate_time(alarm_time.day, "ALARM day", 1, number_of_days_in_month(month, year));

            // write alarm time values to chip
            std::vector<uint8_t> data{ static_cast<uint8_t>(Rtc8563Register::MinuteAlarm) };
            data.push_back(decimal_to_bcd(alarm_time.minute)
                | static_cast<uint8_t>(alarm_time.ena_alrm_minute ? 0x00 : 0x80));

            data.push_back(decimal_to_bcd(alarm_time.hour24)
                | static_cast<uint8_t>(alarm_time.ena_alrm_hour ? 0x00 : 0x80));

            data.push_back(decimal_to_bcd(alarm_time.day)
                | static_cast<uint8_t>(alarm_time.ena_alrm_day ? 0x00 : 0x80));

            data.push_back(static_cast<uint8_t>(alarm_time.weekday)
                | static_cast<uint8_t>(alarm_time.ena_alrm_weekday ? 0x00 : 0x80));

            res &= write(address, data);
        }

        return res;
    }

    // Check to see if alarm flag is active
    bool PCF8563::is_alarm_flag_active(bool& alarm_flag)
    {
        util::FixedBuffer<uint8_t, 1> rd_data;
        bool res = read(address, static_cast<uint8_t>(Rtc8563Register::ControlStatus2), rd_data);
        alarm_flag = (rd_data[0] & 0x08) >> 3;

        return res;
    }

    // Clear alarm flag
    bool PCF8563::clear_alarm_flag()
    {
        util::FixedBuffer<uint8_t, 1> rd_data;
        bool res = read(address, static_cast<uint8_t>(Rtc8563Register::ControlStatus2), rd_data);

        std::vector<uint8_t> data{ static_cast<uint8_t>(Rtc8563Register::ControlStatus2) };
        data.push_back(rd_data[0] & 0x17);

        return res & write(address, data);
    }

    // Get 12 hour time string
    std::string PCF8563::get_12hr_time_string(uint8_t hours_24, uint8_t minutes, uint8_t seconds)
    {
        std::string hrs_str = hours_24 == 0 ? std::to_string(12) : std::to_string(hours_24 % 12);
        std::string am_pm_str = hours_24 < 12 ? " AM" : " PM";

        return hrs_str + add_colon_zero_padding(minutes) + add_colon_zero_padding(seconds) + am_pm_str;
    }

    // Get 24 hour time string
    std::string PCF8563::get_24hr_time_string(uint8_t hours_24, uint8_t minutes, uint8_t seconds)
    {
        return std::to_string(hours_24) + add_colon_zero_padding(minutes) + add_colon_zero_padding(seconds);
    }

    // Add colon and zero padding to number
    std::string PCF8563::add_colon_zero_padding(uint8_t time)
    {
        return time < 10 ? ":0" + std::to_string(time) : ":" + std::to_string(time);
    }

    // Validate time
    void PCF8563::validate_time(uint8_t& time, std::string err_msg, uint8_t min_limit, uint8_t max_limit)
    {
        if ((time > max_limit) | (time < min_limit))
        {
            Log::error(TAG,
            "Error - {} must be between {} and {}, setting to {}",
            err_msg,
            min_limit,
            max_limit,
            min_limit);

            time = min_limit;
        }
    }

    // Validate year
    void PCF8563::validate_year(uint16_t& year)
    {
        if ((year > 2099) | (year < 2000))
        {
            Log::error(TAG, "Error - RTC year must be between 2000 and 2009, setting to 2000");
            year = 2000;
        }
    }
}
