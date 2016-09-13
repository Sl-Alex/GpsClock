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
* @file usr_matrix_hal.h
* @brief Header file for usr_matrix_hal.c
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#ifndef USR_MATRIX_HAL_H
#define USR_MATRIX_HAL_H

#include <stdint.h>
#include "stm32f10x_i2c.h"
#include "usr_ht16k33.h"
#include "usr_font.h"

/// @brief Number of hardware blocks
#define MATRIX_BLOCKS_X  5
#define MATRIX_BLOCKS_Y  3
/// @brief I2C Interface
#define USR_MATRIX_IFACE    I2C2

void usr_matrix_hal_update(uint8_t brightness);
void usr_matrix_hal_set_pix(uint8_t col, uint8_t row, uint8_t data);
void usr_matrix_hal_set_col(uint8_t x, uint8_t value);
void usr_matrix_hal_write_str (uint8_t x, uint8_t y, char * str);
void usr_matrix_hal_clear(uint8_t value);
void usr_matrix_hal_set_brightness(uint8_t brightness);
void usr_matrix_hal_draw_box(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void usr_matrix_draw_digit_big(uint8_t x, uint8_t digit);
void usr_matrix_draw_digit_medium(uint8_t x, uint8_t digit);
void usr_matrix_draw_digit_small(uint8_t x, uint8_t digit);

#endif // USR_MATRIX_HAL_H
