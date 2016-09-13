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
* @file usr_rtc.h
* @brief Header file for usr_rtc.c
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#ifndef USR_RTC_H
#define USR_RTC_H

#include <stdint.h>
#include "stm32f10x_rcc.h"
#include "usr_global.h"
#include "usr_time_utils.h"
#include "FreeRTOS.h"
#include "task.h"

/// @brief Just a magic 16-bit number. Stored in backup register, helps to determine
/// if RTC module has already been configured.
/// @note Can be any value except of zero
#define USR_RTC_CONFIGURED  0xABCD

void usr_rtc_init(void);
void usr_rtc_set(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second);
void usr_rtc_convert(uint32_t rtc_value);
void usr_rtc_get_local_time(void);

#endif // USR_RTC_H
