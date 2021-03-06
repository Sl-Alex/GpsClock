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
* @file usr_brightness.h
* @brief Header file for usr_brightness.c
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#ifndef USR_BMP180_H
#define USR_BMP180_H

#include <stdint.h>
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "FreeRTOS.h"

void usr_bmp180_init(void);
uint32_t usr_bmp180_read_temp(void);
uint32_t usr_bmp180_read_pressure(void);
uint32_t usr_bmp180_get_pres_min(uint8_t index);
uint32_t usr_bmp180_get_pres_max(uint8_t index);

#endif // USR_BMP180_H
