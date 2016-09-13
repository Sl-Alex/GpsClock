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
* @brief Brightness measurement (via ADC triggered from timer)
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include "usr_si7021.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usr_i2c.h"

#define SI7021_ADDR 0x40
#define SI7021_IFACE I2C1

#define SI7021_START_RH_MEASURE_NH      0xF5
#define SI7021_START_TEMP_MEASURE_NH    0xF3
#define SI7021_READ_READY_TEMP          0xE0

void usr_si7021_write(uint8_t byte);

void usr_si7021_init(void)
{
    usr_si7021_start();
}

void usr_si7021_start(void)
{
    usr_si7021_write(SI7021_START_RH_MEASURE_NH);
}

void usr_si7021_write(uint8_t byte)
{
    usr_i2c_transfer(SI7021_IFACE, SI7021_ADDR, &byte, 1, 0, 0);
    vTaskDelay(20);
    return;
}

uint32_t usr_si7021_read(void)
{
    uint8_t data[2];
    uint32_t ret = 0;

    usr_i2c_transfer(SI7021_IFACE, SI7021_ADDR, 0, 0, data, 2);
    ret = (uint32_t)(data[0])<<8;
    ret |= (uint32_t)(data[1]);

    return ret;
}

/// @brief read relative humidity (0..99)
uint32_t usr_si7021_read_hum(void)
{
    uint32_t hum = usr_si7021_read();
    if (hum == 0)
        return 0;

    hum = (hum *125)/65536 - 6;
    if (hum > 99)
        hum = 99;

    return hum;
}

/// @brief read temperature, which was measured during humidity measurement
uint32_t usr_si7021_read_temp(void)
{
    usr_si7021_write(SI7021_READ_READY_TEMP);

    uint32_t temp = usr_si7021_read();

    temp = (175.72*temp)/65536 - 46.85;
    if (temp > 99)
        temp = 99;

    return temp;
}
