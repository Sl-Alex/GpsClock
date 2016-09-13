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
* @file usr_fm24c04.h
* @brief Header file for usr_fm24c04.c
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#ifndef USR_FM24C04_H
#define USR_FM24C04_H

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "usr_global.h"

#define FRAM_LOG_SIZE   24  ///< Size of the FRAM log. Should be more than 1

/// @brief Pressure log header
typedef struct __attribute__((packed)){
    uint32_t valid;          ///< Is this log valid
    TIME_T last_item_start; ///< Start time of the last item (UTC)
    TIME_T last_item_end;   ///< End time of the last item (UTC)
} FRAM_LOG_HEADER_T;

typedef struct __attribute__((packed)){
    uint32_t min;   ///< Minimal pressure
    uint32_t max;   ///< Maximal pressure
} FRAM_LOG_ITEM_T;

typedef struct __attribute__((packed)){
    FRAM_LOG_HEADER_T header;   ///< Header
    FRAM_LOG_ITEM_T data[FRAM_LOG_SIZE];    ///< Data for the last 24 hours
} FRAM_LOG_T;

uint32_t usr_fm24c04_read(uint32_t address, uint8_t * data, uint32_t datasize);
uint32_t usr_fm24c04_write(uint32_t address, uint8_t * data, uint32_t datasize);
void usr_fm24c04_write_data(FRAM_LOG_T * log);
void usr_fm24c04_load_data(FRAM_LOG_T * log);
void usr_fm24c04_add_log(FRAM_LOG_T * log, uint32_t pres, TIME_T * utc_time);

#endif // USR_FM24C04_H
