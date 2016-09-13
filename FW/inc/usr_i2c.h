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

#ifndef USR_I2C_H
#define USR_I2C_H

#include <stdint.h>
#include "stm32f10x.h"

void usr_i2c_init(void);
void usr_i2c_reset(void);
uint32_t usr_i2c_transfer(I2C_TypeDef * interface, uint8_t addr, uint8_t * wr, uint8_t wr_size, uint8_t * rd, uint8_t rd_size);
uint32_t usr_i2c_transfer_2(I2C_TypeDef * interface, uint8_t addr, uint8_t * wr, uint8_t wr_size, uint8_t * wr2, uint8_t wr2_size);

#endif // USR_I2C_H
