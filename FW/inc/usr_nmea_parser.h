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
* @file usr_nmea_parser.h
* @brief Header file for usr_nmea_parser.c
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#ifndef USR_NMEA_PARSER_H
#define USR_NMEA_PARSER_H

#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "usr_ht16k33.h"

#include "stm32f10x_usart.h"

#include "usr_time_utils.h"
#include "usr_global.h"

#define NMEA_BAUDRATE 9600
#define NMEA_NOT_FOUND -1

typedef struct {
    int8_t id;
    int8_t signal;
    int8_t x_coord;
    int8_t y_coord;
} usr_nmea_satellite;

uint8_t usr_nmea_is_valid(void);
void    usr_nmea_valid_clear(void);
void    usr_nmea_parse(uint8_t new_byte);
void    usr_nmea_parse_str(void);
uint8_t usr_nmea_check_str(void);
usr_nmea_satellite * nmea_get_info(uint8_t index);

#endif // USR_NMEA_PARSER_H
