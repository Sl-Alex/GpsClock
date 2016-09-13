/*
    This file is part of "GPS clock" project.

    "GPS clock" is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Some parts of this project are written by 3-rd party vendors under a
    different license. See readme.txt for the complete list of 3-rd party
    source code.

    "GPS clock" is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with "GPS clock".  If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file usr_time_utils.c
* @brief Utils for date/time calculation, UTC to local conversion, etc.
*
* @date 05.06.2014 First documented version
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include <stdlib.h>
#include "usr_time_utils.h"
#include "FreeRTOS.h"
#include "task.h"

TIME_T tmp_time;

/// @brief Calculates day of week
/// @param[in] year - full year (2014, for example)
/// @param[in] month - Month number (1..12)
/// @param[in] day - Day of month (1..31)
/// @return 0 for Sunday
/// @return 1..6 for Monday..Saturday
uint32_t usr_time_utils_get_day_of_week(uint32_t year, uint32_t month, uint32_t day)
{
    uint32_t a1 = (14 - month) / 12;
    uint32_t a2 = year - a1;
    uint32_t a3 = month + 12 * a1 - 2;

    return (day + a2 + a2 / 4 - a2 / 100 + a2 / 400 + (31 * a3) / 12) % 7;
}

int32_t usr_time_utils_compare(TIME_T * time1, TIME_T * time2)
{
    if (time1->year > time2->year)
        return 1;
    if (time1->year < time2->year)
        return -1;
    if (time1->month > time2->month)
        return 1;
    if (time1->month < time2->month)
        return -1;
    if (time1->day > time2->day)
        return 1;
    if (time1->day < time2->day)
        return -1;
    if (time1->hour > time2->hour)
        return 1;
    if (time1->hour < time2->hour)
        return -1;
    if (time1->minute > time2->minute)
        return 1;
    if (time1->minute < time2->minute)
        return -1;
    if (time1->second > time2->second)
        return 1;
    if (time1->second < time2->second)
        return -1;
    // Everything is the same
    return 0;
}

/// @brief Is current year leap
/// @param[in] year - full year (2014, for example)
/// @return 1 for leap years, 0 otherwise
uint32_t usr_time_utils_is_leap(uint32_t year)
{
    if (year % 100)
    {
        if (year % 400)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if (year % 4)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

/// @brief Calculate number of days in specified month
/// @param[in] year - full year (2014, for example)
/// @param[in] month - Month number (1..12)
/// @return number of days (28..31)
uint32_t usr_time_utils_get_days_of_month(uint32_t year, uint32_t month)
{
    if ((month ==  1) || (month ==  3) || (month ==  5) || (month ==  7) || // Jan, Mar, May, Jul
        (month ==  8) || (month == 10) || (month == 12))                    // Aug, Oct, Dec
        return 31;
    if ((month ==  4) || (month ==  6) || (month ==  9) || (month == 11))   // Apr, Jun, Sep, Nov
        return 30;
    if (usr_time_utils_is_leap(year)) // Feb
        return 29;
    else
        return 28;
}

void usr_time_utils_get_local_datetime(TIME_T * pUTC, TIME_T * pLOC, uint32_t offset)
{
    pLOC->year  = pUTC->year;
    pLOC->month = pUTC->month;
    pLOC->day   = pUTC->day;
    pLOC->hour  = pUTC->hour + offset;

    if (pLOC->hour > 23)
    {
        pLOC->hour %= 24;
        pLOC->day++;
        if (pLOC->day > usr_time_utils_get_days_of_month(pLOC->year, pLOC->month))
        {
            pLOC->day = 1;
            pLOC->month++;
            if (pLOC->month > 12)
            {
                pLOC->month = 1;
                pLOC->year++;
            }
        }
    }
}

/// @brief Checks if current time is summer time or not
/// @note UTC time must be used
/// @details Summertime is from last Sunday of March (from 1 hour UTC) till last Sunday of October (till 1 hour UTC)
/// @param[in] year - full year (2014, for example)
/// @param[in] month - Month number (1..12)
/// @param[in] day - Day of month (1..31)
/// @param[in] hour - Hour of day (UTC)
/// @return 1 if summertime is active
/// @return 0 otherwise
uint8_t usr_time_utils_check_summer_time(TIME_T * pTime)
{
    // Last Sunday of March
    uint32_t lms = 31 - usr_time_utils_get_day_of_week(pTime->year,03,31);
    // Last Sunday of October
    uint32_t los = 31 - usr_time_utils_get_day_of_week(pTime->year,10,31);
    // Check if it is less than the last Sunday of March
    if (pTime->month < 3) // Not March yet
    {
        return 0;
    }
    else if (pTime->month == 3) // March
    {
        if (pTime->day < lms) // Before the last Sunday of March
        {
            return 0;
        }
        else if ((pTime->day == lms) && (pTime->hour < 1)) // Last Sunday of March, but before 1 hour (UTC)
        {
            return 0;
        }
        else // Last Sunday of March, more or equal to 3 hours
        {
            return 1;
        }
    }
    else if ((pTime->month > 3) && (pTime->month < 10)) // More than March, but less than October
    {
        return 1;
    }
    else if (pTime->month == 10) // October
    {
        if (pTime->day < los) // Before the last Sunday of October
        {
            return 1;
        }
        else if ((pTime->day == los) && (pTime->hour < 1)) // Last Sunday of October, but before 1 hour (UTC)
        {
            return 1;
        }
        else // After 1 hour of last Sunday of October
        {
            return 0;
        }
    }
    else // After October
    {
        return 0;
    }
}

/// @brief Calculate difference in seconds
uint32_t usr_time_utils_need_sync(TIME_T * time_1, TIME_T * time_2)
{
    taskENTER_CRITICAL();

    // Сравнить минуты
    // Если был переход через час - пропустить синхронизацию
    // Если разница минут больше единицы - надо синхронизировать
    // Если часы отличаются - синхронизировать!
    // Если дни отличаются - синхронизировать!
    // Если месяцы отличаются - синхронизировать!
    // Если годы отличаются - синхронизировать!

    // Skip sync if there is a change of hour
    if (((time_1->minute == 59) && (time_2->minute == 0)) ||
        ((time_1->minute == 0) && (time_2->minute == 59)))
    {
        taskEXIT_CRITICAL();
        return 0;
    }

    // Check the rest
    if ((abs(time_1->minute - time_2->minute) > 1) ||
        (time_1->hour != time_2->hour) ||
        (time_1->day != time_2->day) ||
        (time_1->month != time_2->month) ||
        (time_1->year != time_2->year))
    {
        taskEXIT_CRITICAL();
        return 1;
    }
    else
    {
        taskEXIT_CRITICAL();
        return 0;
    }

}

/// @brief Checks if the difference between two TIME_T values is more or equal to 1 hour
/// start must be more or equal to end
uint32_t usr_time_utils_hour_diff(TIME_T * start, TIME_T * end)
{
    // If we have to check for NY
    if ((start->year + 1) == end->year)
    {
        // Is it really NY
        if ((start->month == 12) && (end->month == 1) &&
            (start->day == 31) && (end->day == 1))
        {
            // Check hours
            if (end->hour < start->hour)
                // Normal difference
                return (24 - start->hour + end->hour);
            else
                // More than 24 hours
                return UINT32_MAX;
        }
        else
            return UINT32_MAX;
    }
    else if (start->year != end->year)
        // More than 1 year difference
        return UINT32_MAX;

    // OK, year is the same
    // Let's check month transition
    if (((start->month + 1) == end->month) &&
        (start->day == usr_time_utils_get_days_of_month(start->year,start->month)) &&
        (end->day == 1))
    {
        // Check hours
        if (end->hour < start->hour)
            // Normal difference
            return (24 - start->hour + end->hour);
        else
            // More than 24 hours
            return UINT32_MAX;
    }
    else if (start->month != end->month)
        // More than 1 month difference
        return UINT32_MAX;

    // OK, year and month are the same
    // Let's check day transition
    if ((start->day + 1) == end->day)
    {
        // Exactly 1 day difference
        // Check hours
        if (end->hour < start->hour)
            // Normal difference
            return (24 - start->hour + end->hour);
        else
            // More than 24 hours
            return UINT32_MAX;
    }
    else if (start->day != end->day)
        // More than 1 day difference
        return UINT32_MAX;

    // OK, year, month, and even day are the same
    // Let's check time difference
    if (end->hour < start->hour)
        // Error, end is earlier than start
        return UINT32_MAX;

    // Normal difference
    return (end->hour - start->hour);
}
