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
* @file usr_matrix_hal.c
* @brief Driver of the whole LED matrix
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include "usr_matrix_hal.h"

typedef struct {
    I2C_TypeDef * interface;
    uint8_t addr_off;
} matrix_config_t;

// Left to right, bottom to top
matrix_config_t matrix_config[MATRIX_BLOCKS_X * MATRIX_BLOCKS_Y] = {
    {I2C2,0}, {I2C2,1}, {I2C2,2}, {I2C2,3}, {I2C2,4},
    {I2C2,5}, {I2C2,6}, {I2C2,7}, {I2C1,5}, {I2C1,6},
    {I2C1,0}, {I2C1,1}, {I2C1,2}, {I2C1,3}, {I2C1,4}
};

/// @brief Display memory (bytes left to right, bits top to bottom)
/// 8 * 80 total resolution
static uint8_t matrix_ram[MATRIX_BLOCKS_Y][16 * MATRIX_BLOCKS_X];

void usr_matrix_hal_update(uint8_t brightness)
{
//    usr_matrix_hal_clear(0x00);
    if (brightness > 15) brightness = 15;

    // Configure all connected blocks
    for (uint8_t i = 0; i < MATRIX_BLOCKS_X * MATRIX_BLOCKS_Y; i++)
    {
        usr_ht16k33_write(matrix_config[i].interface, USR_HT16K33_DEFAULT_ADDR + matrix_config[i].addr_off, USR_HT16K33_ON);        // turn on device oscillator
    }

    for(uint8_t y = 0; y < MATRIX_BLOCKS_Y; y++)
    for (uint8_t x = 0; x < MATRIX_BLOCKS_X; x++)
    {
        usr_ht16k33_update(matrix_config[y * MATRIX_BLOCKS_X + x].interface, USR_HT16K33_DEFAULT_ADDR + matrix_config[y * MATRIX_BLOCKS_X + x].addr_off, &matrix_ram[y][16*x]);
    }

    for (uint8_t i = 0; i < MATRIX_BLOCKS_X * MATRIX_BLOCKS_Y; i++)
    {
        usr_ht16k33_write(matrix_config[i].interface, USR_HT16K33_DEFAULT_ADDR + matrix_config[i].addr_off, USR_HT16K33_DISPLAYON); // turn on display, no blink
        usr_ht16k33_write(matrix_config[i].interface, USR_HT16K33_DEFAULT_ADDR + matrix_config[i].addr_off, USR_HT16K33_DIM + brightness);   // set brightness
    }
}

void usr_matrix_hal_set_brightness(uint8_t brightness)
{
    for (uint8_t i = 0; i < MATRIX_BLOCKS_X * MATRIX_BLOCKS_Y; i++)
    {
        usr_ht16k33_write(matrix_config[i].interface, USR_HT16K33_DEFAULT_ADDR + matrix_config[i].addr_off, USR_HT16K33_DIM + brightness);   // set min brightness
    }
}

void usr_matrix_hal_write_str (uint8_t x, uint8_t y, char * str)
{
    uint8_t i = 0;
    uint8_t tmp;

    while (str[i] != 0)
    {
        if ((y % 8) == 0)
        {
            // Everything fits vertically in one block
            for (uint8_t j = 0; j < 5; j++)
            {
                if (x >= (16 * MATRIX_BLOCKS_X)) continue;
                matrix_ram[y/8][x++] = usr_font[(uint8_t)str[i]][j];
            }
            matrix_ram[y/8][x++] = 0x00;
        }
        else
        {
            uint8_t x_start = x;
            // Does not fit vertically in one block
            // Draw first (low) block
            for (uint8_t j = 0; j < 5; j++)
            {
                if ((y/8) >= MATRIX_BLOCKS_Y) continue;
                if (x >= (16 * MATRIX_BLOCKS_X)) continue;

                tmp = matrix_ram[y/8][x] & (0xFF >> (8 - (y%8)));

                matrix_ram[y/8][x] = tmp | usr_font[(uint8_t)str[i]][j]<<(y%8);

                x++;
            }
            if ((x < (16 * MATRIX_BLOCKS_X)) &&
                ((y/8) < MATRIX_BLOCKS_Y))
            {
                tmp = matrix_ram[y/8][x] & (0xFF >> (8 - (y%8)));
                matrix_ram[y/8][x] = tmp;
            }
            x = x_start;
            // Draw second (top) block
            for (uint8_t j = 0; j < 5; j++)
            {
                if ((y/8+1) >= MATRIX_BLOCKS_Y)
                {
                    x++;
                    continue;
                }
                if (x >= (16 * MATRIX_BLOCKS_X))
                {
                    x++;
                    continue;
                }

                tmp = matrix_ram[y/8 + 1][x] & (0xFF << (y%8));

                matrix_ram[y/8+1][x] = tmp | usr_font[(uint8_t)str[i]][j]>>(8-(y%8));
                x++;
            }
            if ((x < (16 * MATRIX_BLOCKS_X)) &&
                ((y/8+1) < MATRIX_BLOCKS_Y))
            {
                tmp = matrix_ram[y/8 + 1][x] & (0xFF << (y%8));
                matrix_ram[y/8+1][x] = tmp;
            }
            x++;
            //matrix_ram[y/8][x++] = 0x00;
        }
        //if (x >= (16 * MATRIX_BLOCKS_X)) return;
        i++;
    }
}

void usr_matrix_draw_digit_big(uint8_t x, uint8_t digit)
{
    for (uint8_t j = 0; j < 11; j++)
    {
        if (x >= (16 * MATRIX_BLOCKS_X)) continue;
        matrix_ram[1][x] = usr_digits_big[digit][j*2];
        matrix_ram[0][x] = usr_digits_big[digit][j*2 + 1];
        x++;
    }
}

void usr_matrix_draw_digit_small(uint8_t x, uint8_t digit)
{
    for (uint8_t j = 0; j < 3; j++)
    {
        if (x >= (16 * MATRIX_BLOCKS_X)) continue;
        matrix_ram[0][x] |= usr_digits_small[digit][j]<<5;
        matrix_ram[1][x] |= usr_digits_small[digit][j]>>3;
        x++;
    }
}

void usr_matrix_draw_digit_medium(uint8_t x, uint8_t digit)
{
    uint8_t tmp;

    for (uint8_t j = 0; j < 6; j++)
    {
        if (x >= (16 * MATRIX_BLOCKS_X)) continue;

        matrix_ram[1][x] = usr_digits_medium[digit][j*2];
        tmp = matrix_ram[0][x] & 0x7F;
        matrix_ram[0][x] = tmp | usr_digits_medium[digit][j*2 + 1];

        x++;
    }
}

void usr_matrix_hal_set_pix(uint8_t x, uint8_t y, uint8_t value)
{
    if (x >= 16 * MATRIX_BLOCKS_X) return;
    if (y >= 8 * MATRIX_BLOCKS_Y) return;
    if (value)
        matrix_ram[y/8][x] |= (1 << (y%8));
    else
        matrix_ram[y/8][x] &=~(1 << (y%8));
}

void usr_matrix_hal_set_col(uint8_t x, uint8_t value)
{
    if (x >= (16 * MATRIX_BLOCKS_X)) return;

    if (value > 0)
    {
        for (uint8_t y = 0; y < MATRIX_BLOCKS_Y; y++)
            matrix_ram[y][x] = 0xFF;
    }
    else
    {
        for (uint8_t y = 0; y < MATRIX_BLOCKS_Y; y++)
            matrix_ram[y][x] = 0;
    }
}

void usr_matrix_hal_set_row(uint8_t y, uint8_t value)
{
    if ((y/8) >= MATRIX_BLOCKS_Y) return;

    if (value > 0)
    {
        for (uint8_t x = 0; x < MATRIX_BLOCKS_X *16; x++)
            matrix_ram[y/8][x] |= (0x01<<(y%8));
    }
    else
    {
        for (uint8_t x = 0; x < MATRIX_BLOCKS_X *16; x++)
            matrix_ram[y/8][x] &=~(0x01<<(y%8));
    }
}

void usr_matrix_hal_clear(uint8_t value)
{
    for (uint8_t y = 0; y < MATRIX_BLOCKS_Y; y++)
    {
        for (uint8_t x = 0; x < 16 * MATRIX_BLOCKS_X; x++)
        {
            matrix_ram[y][x] = value;
        }
    }
}

void usr_matrix_hal_draw_box(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    for (uint8_t i = x1; i <= x2; i++)
    {
        usr_matrix_hal_set_pix(i,y1,1);
    }
    for (uint8_t i = x1; i <= x2; i++)
    {
        usr_matrix_hal_set_pix(i,y2,1);
    }
    for (uint8_t i = y1; i <= y2; i++)
    {
        usr_matrix_hal_set_pix(x1,i,1);
    }
    for (uint8_t i = y1; i <= y2; i++)
    {
        usr_matrix_hal_set_pix(x2,i,1);
    }
}
