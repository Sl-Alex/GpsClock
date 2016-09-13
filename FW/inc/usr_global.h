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
* @file usr_global.h
* @brief Header file for usr_global.c
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#ifndef USR_GLOBAL_H
#define USR_GLOBAL_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "usr_time_utils.h" // TIME_T structure definition

// Define global variables
extern TIME_T  gps_time;
extern TIME_T  rtc_time;
extern TIME_T  loc_time;

extern QueueHandle_t xQueueNMEA;
extern QueueHandle_t xQueueClock;
extern QueueHandle_t xQueueBrightness;

extern SemaphoreHandle_t xSemaphoreDisplayUpdate;

#endif // USR_GLOBAL_H
