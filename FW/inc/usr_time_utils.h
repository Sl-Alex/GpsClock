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
* @file usr_time_utils.h
* @brief Header file for usr_time_utils.c
*
* @date 05.06.2014 First documented version
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#ifndef USR_TIME_UTILS_H
#define USR_TIME_UTILS_H

#include "stdint.h"
//#include "usr_global.h"

/// @brief Holds time and date
typedef struct __attribute__((packed)) {
    uint32_t hour;      ///< Hour (0..23)
    uint32_t minute;    ///< Minute (0..59)
    uint32_t second;    ///< Second (0..59)
    uint32_t day;       ///< Day (1..28-31)
    uint32_t month;     ///< Month (1..12)
    uint32_t year;      ///< Year (i.e. 2014)
} TIME_T;

// Calculates day of week
uint32_t usr_time_utils_get_day_of_week(uint32_t year, uint32_t month, uint32_t day);
// Compare two times (>0 when time1>time2, == 0 when they are the same, <0 when time1<time2)
int32_t usr_time_utils_compare(TIME_T * time1, TIME_T * time2);
// Is current year leap
uint32_t usr_time_utils_is_leap(uint32_t year);
// Calculate number of days in specified month
uint32_t usr_time_utils_get_days_of_month(uint32_t year, uint32_t month);
// Calculate number of days in specified month
void usr_time_utils_get_local_datetime(TIME_T * pUTC, TIME_T * pLOC, uint32_t offset);
// Checks if current time is summer time or not
uint8_t usr_time_utils_check_summer_time(TIME_T * pTime);
uint32_t usr_time_utils_need_sync(TIME_T * time_1, TIME_T * time_2);
uint32_t usr_time_utils_hour_diff(TIME_T * start, TIME_T * end);

#endif // USR_TIME_UTILS_H
