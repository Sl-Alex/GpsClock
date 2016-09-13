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
* @file usr_ht16k33.c
* @brief LED matrix controller driver
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include "usr_ht16k33.h"
#include "usr_i2c.h"

static const uint8_t col_decode [8] = {4,2,1,7,6,0,5,3};
static const uint8_t row_decode1[8] = {2,0,7,1,3,6,4,5};
static const uint8_t row_decode2[8] = {5,7,4,6,0,3,1,2};
//static const uint8_t col_decode [8] = {3,5,6,4,2,7,1,0};
//static const uint8_t row_decode1[8] = {5,6,4,7,1,3,2,0};
//static const uint8_t row_decode2[8] = {3,5,4,6,2,7,0,1};

static uint8_t ht16k33_data[17];

static uint8_t row_dec(uint8_t data, uint8_t disp_num)
{
    uint8_t res = 0;
    switch (disp_num)
    {
        case 0:
            for (uint8_t i = 0; i < 8; i++)
                if (data & (1 << i)) res |= (1 << row_decode1[i]);
            return res;
        case 1:
            for (uint8_t i = 0; i < 8; i++)
                if (data & (1 << i)) res |= (1 << row_decode2[i]);
            return res;
    }
    return 0;
}

void usr_ht16k33_write(I2C_TypeDef * interface, uint8_t dev_addr, uint8_t byte)
{
    usr_i2c_transfer(interface, dev_addr, &byte, 1, 0, 0);
}

void usr_ht16k33_update(I2C_TypeDef * interface, uint8_t dev_addr, uint8_t * image)
{
    int i;

    // First cell is a start address, should be zero
    ht16k33_data[0] = 0;

    // Fill the rest with the display data
    for (i = 0; i < 16; i++)
        ht16k33_data[i+1] = row_dec(image[col_decode[ i /2 ] + (1 - i % 2 ) * 8 ], 1 - i % 2);

    // Write to the driver
    usr_i2c_transfer(interface, dev_addr, ht16k33_data, 17, 0, 0);
    return;
}
