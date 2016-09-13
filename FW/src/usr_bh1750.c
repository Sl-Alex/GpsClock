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
* @file usr_brightness.c
* @brief Brightness measurement
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include "usr_bh1750.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usr_i2c.h"

#define BH1750_ADDR 0x23
//#define BH1750_ADDR 0x5C
#define BH1750_IFACE I2C1

#define BH1750_CMD_POWERDOWN 0b00000000 // No active state.
#define BH1750_CMD_POWERON   0b00000001 // Power on and wait for measurement command.
#define BH1750_CMD_RESET     0b00000111 // Reset Data register value
#define BH1750_CMD_HR1_CONT  0b00010000 // Start measurement at 1lx resolution.
#define BH1750_CMD_HR2_CONT  0b00010001 // Start measurement at 0.5lx resolution.
#define BH1750_CMD_LOW_CONT  0b00010011 // Start measurement at 4lx resolution.
#define BH1750_CMD_HR1_ONCE  0b00100000 // Start measurement at 1lx resolution.
#define BH1750_CMD_HR2_ONCE  0b00100001 // Start measurement at 0.5lx resolution.
#define BH1750_CMD_LOW_ONCE  0b00100011 // Start measurement at 4lx resolution.

void usr_bh1750_init(void)
{
    usr_bh1750_write(BH1750_CMD_POWERON);
    usr_bh1750_write(BH1750_CMD_HR2_ONCE);
}

void usr_bh1750_write(uint8_t byte)
{
    usr_i2c_transfer(BH1750_IFACE, BH1750_ADDR, &byte, 1, 0, 0);
}

uint32_t usr_bh1750_read(void)
{
    uint8_t data[2];
    uint32_t ret = UINT32_MAX;

    // Read two bytes brightness
    if (usr_i2c_transfer(BH1750_IFACE, BH1750_ADDR, 0, 0, data, 2) != 0)
    {
        ret = data[0] << 8;
        ret |= data[1];
    }

    //vTaskDelay(200);
    // Reinitialize and start next measurement
    usr_bh1750_write(BH1750_CMD_POWERON);
    usr_bh1750_write(BH1750_CMD_HR2_ONCE);
    //vTaskDelay(200);
    return ret;
}
