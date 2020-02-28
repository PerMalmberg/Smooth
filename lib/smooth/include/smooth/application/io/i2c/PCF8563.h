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

#include "smooth/core/io/i2c/I2CMasterDevice.h"
#include "smooth/core/util/FixedBuffer.h"

namespace smooth::application::sensor
{
    class PCF8563
        : public core::io::i2c::I2CMasterDevice
    {
        public:
            enum class Rtc8563Register : uint8_t
            {
                ControlStatus1 = 0x00,
                ControlStatus2,
                Seconds,
                Minutes,
                Hours,
                Days,
                Weekdays,
                Months,
                Years,
                MinuteAlarm,
                HourAlarm,
                DayAlarm,
                WeekdayAlarm,
                ClockOutControl,
                TimerControl,
                Timer
            };

            enum class DayOfWeek : uint8_t
            {
                Sunday,
                Monday,
                Tuesday,
                Wednesday,
                Thursday,
                Friday,
                Saturday
            };

            static const constexpr char* DayOfWeekStrings[] =
            { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

            enum class Month : uint8_t
            {
                January,
                February,
                March,
                April,
                May,
                June,
                July,
                August,
                September,
                October,
                November,
                December
            };

            static const constexpr char* MonthStrings[] =
            { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

            struct RtcTime
            {
                uint8_t seconds;        // 0-59
                uint8_t minutes;        // 0-59
                uint8_t hours24;        // 0-23
                uint8_t days;           // 1-31 depending upon month
                DayOfWeek weekdays;     // Sunday-Saturday
                Month months;           // January-December
                uint16_t years;         // 2000-2099
            };

            struct AlarmTime
            {
                bool ena_alrm_minute;   // true = enable, false = disable
                uint8_t minute;         // 0-59
                bool ena_alrm_hour;     // true = enable, false = disable
                uint8_t hour24;         // 0-23
                bool ena_alrm_day;      // true = enable, false = disable
                uint8_t day;            // 1-31 depending upon month
                bool ena_alrm_weekday;  // true = enable, false = disable
                DayOfWeek weekday;      // Sunday-Saturday
            };

            PCF8563(i2c_port_t port, uint8_t address, std::mutex& guard);

            /// Get the rtc time
            /// \param rtc_time The RtcTime struct that will contain the time data
            /// \return true on success, false on failure.
            bool get_rtc_time(RtcTime& rtc_time);

            /// Set the rtc time
            /// \param rtc_time The RtcTime struct that contains the time data
            /// \return true on success, false on failure.
            bool set_rtc_time(RtcTime& rtc_time);

            /// Get the alarm time
            /// \param alarm_time The AlarmTime struct that will contain the time data
            /// \return true on success, false on failure.
            bool get_alarm_time(AlarmTime& alarm_time);

            /// Set the alarm time
            /// \param alarm_time The AlarmTime struct that contains the time data
            /// \return true on success, false on failure.
            bool set_alarm_time(AlarmTime& alarm_time);

            /// Is alarm flag active - poll this function to see if alarm time has triggered
            /// \param alarm_flag If true alarm is active if false alarm id decactivated
            /// \return true on success, false on failure.
            bool is_alarm_flag_active(bool& alarm_flag);

            /// Clear the alarm flag
            /// \return true on success, false on failure.
            bool clear_alarm_flag();

            /// Get the 12 hour time string
            /// \param hours_24 The decimal 24 hours time
            /// \param minutes The decimal minutes time
            /// \param seconds The decimal seconds time
            /// \return Return the formated time string - hr:min:sec am/pm
            std::string get_12hr_time_string(uint8_t hours_24, uint8_t minutes, uint8_t seconds);

            /// Get the 24 hour time string
            /// \param hours_24 The decimal 24 hours time
            /// \param minutes The decimal minutes time
            /// \param seconds The decimal seconds time
            /// \return Return the formated time string - hr:min:sec
            std::string get_24hr_time_string(uint8_t hours_24, uint8_t minutes, uint8_t seconds);

        private:
            /// BCD to decimal
            /// \param bcd The bcd value to be convert to decimal
            /// \return Return the decimal value of bcd
            uint8_t bcd_to_decimal(uint8_t bcd);

            /// Decimal to BCD
            /// \param decimal The decimal value to be converted to BCD
            /// \return Return the BCD value of decimal
            uint8_t decimal_to_bcd(uint8_t decimal);

            /// Number of days in a month
            /// \param month The month used to determine the number of days in the month
            /// \param year The year used to determine the number of days in the month
            /// \return Return the number of days in the month
            uint8_t number_of_days_in_month(Month month, uint16_t year);

            /// Add leading colon and zero padding if necessary
            /// \param time The time unit to add colon and pad with zero if necessary
            /// \return Return the colon and zero padded number in string format
            std::string add_colon_zero_padding(uint8_t time);

            /// Validate the unit of time
            /// \param time The unit of time to validate
            /// \param err_msg The text string to place at the start of the error message
            /// \param min_limit The minimum time unit amount
            /// \param max_limit The maximum time unit amoint
            void validate_time(uint8_t& time, std::string err_msg, uint8_t min_limit, uint8_t max_limit);

            /// Validate year
            /// \param year The year value that will be validated
            void validate_year(uint16_t& year);
    };
}
