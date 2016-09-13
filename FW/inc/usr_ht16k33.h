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
* @file usr_ht16k33.h
* @brief Header file for usr_ht16k33.c
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#ifndef USR_HT16K33_H
#define USR_HT16K33_H

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"

#define USR_HT16K33_DEFAULT_ADDR    0x70 // I2C bus address for Ht16K33 backpack
#define USR_HT16K33_ON              0x21 // turn device oscillator on
#define USR_HT16K33_STANDBY         0x20 // turn device oscillator off
#define USR_HT16K33_DISPLAYON       0x81 // turn on output pins
#define USR_HT16K33_DISPLAYOFF      0x80 // turn off output pins
#define USR_HT16K33_BLINKON         0x85 // blink rate 1 Hz (-2 for 2 Hz)
#define USR_HT16K33_BLINKOFF        0x81 // same as display on
#define USR_HT16K33_DIM             0xE0 // add level (15=max) to byte

/// @brief set brightness
/// @param[in] dev_addr - Device address (@ref USR_HT16K33_DEFAULT_ADDR + 0..7)
/// @param[in] brightness - Brightness to set (0..15)
#define usr_ht16k33_set_brightness(dev_addr, brightness)   usr_ht16k33_WriteByte(dev_addr, USR_HT16K33_DIM + ((brightness)&0x0F)); // set max brightness

void usr_ht16k33_update(I2C_TypeDef * interface, uint8_t dev_addr, uint8_t * image);
void usr_ht16k33_write(I2C_TypeDef * interface, uint8_t dev_addr, uint8_t byte);

#endif // USR_HT16K33_H
