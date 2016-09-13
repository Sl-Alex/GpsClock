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
* @file usr_global.c
* @brief Global variables, structures, arrays and so on
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include "usr_global.h"

QueueHandle_t xQueueNMEA;
QueueHandle_t xQueueClock;

SemaphoreHandle_t xSemaphoreDisplayUpdate;

/// @brief GPS time (UTC)
TIME_T  gps_time = {0, 0, 0, 0, 0, 0};
/// @brief HW RTC time synchronized with @ref gps_time
TIME_T  rtc_time = {0, 0, 0, 0, 0, 0};
/// @brief Local time according to time zone
TIME_T  loc_time = {0, 0, 0, 0, 0, 0};
