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
* @file usr_bmp180.c
* @brief Pressure/temperature measurement
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2015
*/

#include "usr_bmp180.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usr_i2c.h"

#define BMP180_ADDR 0x77
#define BMP180_IFACE I2C1

//#define BH1750_CMD_POWERDOWN 0b00000000 // No active state.
#define BMP180_CAL_AC1           0xAA  // R   Calibration data (16 bits)
#define BMP180_CAL_AC2           0xAC  // R   Calibration data (16 bits)
#define BMP180_CAL_AC3           0xAE  // R   Calibration data (16 bits)
#define BMP180_CAL_AC4           0xB0  // R   Calibration data (16 bits)
#define BMP180_CAL_AC5           0xB2  // R   Calibration data (16 bits)
#define BMP180_CAL_AC6           0xB4  // R   Calibration data (16 bits)
#define BMP180_CAL_B1            0xB6  // R   Calibration data (16 bits)
#define BMP180_CAL_B2            0xB8  // R   Calibration data (16 bits)
#define BMP180_CAL_MB            0xBA  // R   Calibration data (16 bits)
#define BMP180_CAL_MC            0xBC  // R   Calibration data (16 bits)
#define BMP180_CAL_MD            0xBE  // R   Calibration data (16 bits)

#define BMP180_CTRL              0xF4  // Control register
#define BMP180_CONV_RES          0xF6  // Conversion result

#define BMP180_CTRL_TMP          0x2E
#define BMP180_CTRL_PR0          0x34
#define BMP180_CTRL_PR1          0x74
#define BMP180_CTRL_PR2          0xB4
#define BMP180_CTRL_PR3          0xF4

static int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
static uint16_t ac4, ac5, ac6;

uint16_t usr_bmp180_read16(uint8_t reg);
void usr_bmp180_read_calibration(void);
void usr_bmp180_add_pres(uint32_t pres);

void usr_bmp180_init(void)
{
//    usr_bmp180_write(BMP180_IFACE, BH1750_CMD_POWERON);
//    usr_bmp180_write(BMP180_IFACE, BH1750_CMD_HR2_CONT);
//    vTaskDelay(200);
    usr_bmp180_read_calibration();
}

void usr_bmp180_write(uint8_t byte)
{
    usr_i2c_transfer(BMP180_IFACE, BMP180_ADDR, &byte, 1, 0, 0);
}

void usr_bmp180_read_calibration(void)
{
  mb = usr_bmp180_read16(0xD0);

  ac1 = usr_bmp180_read16(BMP180_CAL_AC1);
  ac2 = usr_bmp180_read16(BMP180_CAL_AC2);
  ac3 = usr_bmp180_read16(BMP180_CAL_AC3);
  ac4 = usr_bmp180_read16(BMP180_CAL_AC4);
  ac5 = usr_bmp180_read16(BMP180_CAL_AC5);
  ac6 = usr_bmp180_read16(BMP180_CAL_AC6);

  b1 = usr_bmp180_read16(BMP180_CAL_B1);
  b2 = usr_bmp180_read16(BMP180_CAL_B2);

  mb = usr_bmp180_read16(BMP180_CAL_MB);
  mc = usr_bmp180_read16(BMP180_CAL_MC);
  md = usr_bmp180_read16(BMP180_CAL_MD);
}

uint16_t usr_bmp180_read16(uint8_t reg)
{
    uint8_t tdata[3];
    uint8_t rdata[3];
    uint32_t ret = 0;

    tdata[0] = reg;

    if (usr_i2c_transfer(BMP180_IFACE, BMP180_ADDR, tdata, 1, rdata, 2) != 0)
    {
        ret = (uint32_t)(rdata[0])<<8;
        ret |= (uint32_t)(rdata[1]);

    }

    return ret;
}

uint32_t usr_bmp180_read_temp_raw(void)
{
    uint8_t tdata[3];
    uint8_t rdata[3];
    uint32_t ret = 0;

    tdata[0] = BMP180_CTRL;
    tdata[1] = BMP180_CTRL_TMP;

    if (usr_i2c_transfer(BMP180_IFACE, BMP180_ADDR, tdata, 2, rdata, 0) != 0)
    {
    }
    vTaskDelay(6);

    tdata[0] = BMP180_CONV_RES;

    if (usr_i2c_transfer(BMP180_IFACE, BMP180_ADDR, tdata, 1, rdata, 2) != 0)
    {
        ret  = rdata[0] << 8;
        ret |= rdata[1];
    }
    return ret;
}

uint32_t usr_bmp180_read_pres_raw(void)
{
    uint8_t tdata[3];
    uint8_t rdata[3];
    uint32_t ret = 0;

    tdata[0] = BMP180_CTRL;
    tdata[1] = BMP180_CTRL_PR0;

    if (usr_i2c_transfer(BMP180_IFACE, BMP180_ADDR, tdata, 2, rdata, 0) == 0)
    {
        return 0;
    }
    vTaskDelay(6);

    tdata[0] = BMP180_CONV_RES;

    if (usr_i2c_transfer(BMP180_IFACE, BMP180_ADDR, tdata, 1, rdata, 2) != 0)
    {
        ret  = rdata[0] << 8;
        ret |= rdata[1];
    }
    return ret;
}

int32_t computeB5(int32_t UT) {
    int32_t X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) >> 15;
    int32_t X2 = ((int32_t)mc << 11) / (X1+(int32_t)md);
    return X1 + X2;
}

uint32_t usr_bmp180_read_pressure()
{
    int32_t UT, UP, B3, B5, B6, X1, X2, X3, p;
    uint32_t B4, B7;

    static uint32_t p_rem = 0;
    static uint32_t offset = 0;

    UT = usr_bmp180_read_temp_raw();
    UP = usr_bmp180_read_pres_raw();

    B5 = computeB5(UT);

    // do pressure calcs
    B6 = B5 - 4000;
    X1 = ((int32_t)b2 * ( (B6 * B6)>>12 )) >> 11;
    X2 = ((int32_t)ac2 * B6) >> 11;
    X3 = X1 + X2;
    B3 = (((int32_t)ac1*4 + X3) + 2) / 4;

    X1 = ((int32_t)ac3 * B6) >> 13;
    X2 = ((int32_t)b1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
    B7 = ((uint32_t)UP - B3) * (uint32_t)( 50000UL );

    if (B7 < 0x80000000) {
        p = (B7 * 2) / B4;
    } else {
        p = (B7 / B4) * 2;
    }
    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;

    p = p + ((X1 + X2 + (int32_t)3791)>>4);
//    p = p *75/10000;
    p = p *75/1000;

    p_rem = p % 10;
    p = p / 10;

    if (p_rem > 7) offset = 1;
    if (p_rem < 3) offset = 0;

    p += offset;

    return p;
}
