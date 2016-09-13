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
* @file usr_nmea_parser.c
* @brief NMEA parser. Gets date/time (UTC), satellites information
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include "usr_nmea_parser.h"

static char rx_str[100] = "\0";
static uint8_t rx_index = 0;

TIME_T tmp_time;
uint8_t tmp_valid = 0;
uint8_t tmp_sat_count = 0;
usr_nmea_satellite satinfo[10];
usr_nmea_satellite satinfo_copy[10];

uint8_t usr_nmea_is_valid(void)
{
    return tmp_valid;
}

void usr_nmea_valid_clear(void)
{
    tmp_valid = 0;
}

char *strtok_ex(char *buf, const char *delim);

void usr_nmea_parse(uint8_t new_byte)
{
    switch(new_byte)
    {
        // New line
        case '$':
            rx_index = 0;
            rx_str[0] = 0;
        break;
        case '\x0D':
            usr_nmea_parse_str();
            rx_str[0] = 0;
        break;
        default:
            if (rx_index < (100 - 1))
            {
                rx_str[rx_index] = new_byte;
                rx_index++;
                rx_str[rx_index] = 0;
            }
    }
}

void usr_nmea_parse_str(void)
{
    uint8_t i,j;
    char * rx_substr;
    // Check checksum
    if (usr_nmea_check_str() == 0)
        return;

    // Extract header
    rx_substr = strtok_ex(rx_str, ",");

    // GPGGA
    if (strcmp(rx_substr, "GPGGA") == 0)
    {
        rx_substr = strtok_ex(0, ",");
        if (strlen(rx_substr) == 10)
        {
            tmp_time.hour   = (rx_substr[0] - 0x30) * 10 + rx_substr[1] - 0x30;
            tmp_time.minute = (rx_substr[2] - 0x30) * 10 + rx_substr[3] - 0x30;
            tmp_time.second = (rx_substr[4] - 0x30) * 10 + rx_substr[5] - 0x30;
        } else return;
        rx_substr = strtok_ex(0, ","); // Latitude
        rx_substr = strtok_ex(0, ","); // N/S
        rx_substr = strtok_ex(0, ","); // Longitude
        rx_substr = strtok_ex(0, ","); // E/W
        rx_substr = strtok_ex(0, ","); // Validity
        if (strlen(rx_substr) == 1)
        {
            if (rx_substr[0] == '1')
            {
//                tmp_valid = 1;
            }
            else
            {
   //             tmp_valid = 0;
            }

        }
        rx_substr = strtok_ex(0, ","); // Number of satellites
        if (strlen(rx_substr) > 0)
        {
            tmp_sat_count = atoi(rx_substr);
        }
        else
            tmp_sat_count = 0;
        return;
    }

    // GPGSA
    if (strcmp(rx_substr, "GPGSA") == 0)
    {
        rx_substr = strtok_ex(0, ","); // Auto/Manual 3D
        rx_substr = strtok_ex(0, ","); // Fix: 1 - none, 2 - 2D, 3 - 3D
        if (strlen(rx_substr) > 0)
        {
//            tmp_valid = atoi(gps_substr) - 1;
        }
        else
//            tmp_valid = 0;
        j = 0;
        for (i = 0; i < 12; i++)
        {
            rx_substr = strtok_ex(0, ","); // Active ID
            if ((strlen(rx_substr) > 0) && (j < 10))
            {
                satinfo[j].id = atoi(rx_substr);
                satinfo[j].signal = 0;
                j++;
            }
        }
        while(j < 10)
        {
            satinfo[j].id = NMEA_NOT_FOUND;
            satinfo[j].signal = 0;
            j++;
        }
        return;
    }

    // GPGSV
    if (strcmp(rx_substr, "GPGSV") == 0)
    {
        uint8_t i = 0;
        uint8_t tmp;
        rx_substr = strtok_ex(0, ","); // Message count
        rx_substr = strtok_ex(0, ","); // Message number
        rx_substr = strtok_ex(0, ","); // Satellite in view
        while (1)
        {
            rx_substr = strtok_ex(0, ","); // Sat ID
            if (rx_substr[0] == 0) break;
            if (strlen(rx_substr) > 0)
            {
                i = 0;
                tmp = atoi(rx_substr);
                while (i < 10)
                {
                    if (satinfo[i].id == tmp)
                        break;
                    else
                        i++;
                }
            }
            rx_substr = strtok_ex(0, ","); // Elevation
            if (rx_substr[0] == 0) break;
            rx_substr = strtok_ex(0, ","); // Azimuth
            if (rx_substr[0] == 0) break;
            rx_substr = strtok_ex(0, ","); // SNR (0..99)
            if ((strlen(rx_substr) > 0) && (i < 10))
                satinfo[i].signal = atoi(rx_substr);
            else
                satinfo[i].signal = 0;
        }
        return;
    }

    // GPRMC,235945.800,
    if (strcmp(rx_substr, "GPRMC") == 0)
    {
        rx_substr = strtok_ex(0, ","); // Time
        if (strlen(rx_substr) == 10)
        {
            tmp_time.hour   = (rx_substr[0] - 0x30) * 10 + rx_substr[1] - 0x30;
            tmp_time.minute = (rx_substr[2] - 0x30) * 10 + rx_substr[3] - 0x30;
            tmp_time.second = (rx_substr[4] - 0x30) * 10 + rx_substr[5] - 0x30;
        } else return;
        rx_substr = strtok_ex(0, ","); // Time
        if (rx_substr[0] == 'A') //
            tmp_valid = 1;
        else
            tmp_valid = 0;
        rx_substr = strtok_ex(0, ","); // Time
        rx_substr = strtok_ex(0, ","); // Time
        rx_substr = strtok_ex(0, ","); // Time
        rx_substr = strtok_ex(0, ","); // Time
        rx_substr = strtok_ex(0, ","); // Time
        rx_substr = strtok_ex(0, ","); // Time
        rx_substr = strtok_ex(0, ","); // Time
        if (strlen(rx_substr) == 6)
        {
            tmp_time.day   = (rx_substr[0] - 0x30) * 10 + rx_substr[1] - 0x30;
            tmp_time.month = (rx_substr[2] - 0x30) * 10 + rx_substr[3] - 0x30;
            tmp_time.year  = 2000 + (rx_substr[4] - 0x30) * 10 + rx_substr[5] - 0x30;
        }
        taskENTER_CRITICAL();
        gps_time = tmp_time;
        taskEXIT_CRITICAL();
    }

    // GPZDA
    if (strcmp(rx_substr, "GPZDA") == 0)
    {
        int i,j;
        int8_t tmp;
        // Sort satellites
        for (i = 0; i < 10; i++)
        {
            for (j = 0; j < 10 - i - 1; j++)
            {
                if ((satinfo[j].id > satinfo[j + 1].id) && (satinfo[j+1].id != NMEA_NOT_FOUND))
                {
                    tmp = satinfo[j].id;
                    satinfo[j].id = satinfo[j+1].id;
                    satinfo[j+1].id = tmp;

                    tmp = satinfo[j].signal;
                    satinfo[j].signal = satinfo[j+1].signal;
                    satinfo[j+1].signal = tmp;
                }
            }
        }
        __disable_irq();
        for (i = 0; i < 10; i++)
        {
            satinfo_copy[i] = satinfo[i];
        }
        __enable_irq();
    }
}

uint8_t usr_nmea_check_str(void)
{
    uint8_t len = strlen(rx_str);
    uint8_t cs, rx_cs = 0;
    uint8_t i;

    // Throw away short strings
    if (len < 9)
        return 0;
    // Throw away strings without checksum field
    if (rx_str[len - 3] != '*')
        return 0;
    // Extract checksum from received string
    rx_cs = strtol(&rx_str[len - 2], 0, 16);
    // Null-terminate the rest
    rx_str[len - 3] = 0;
    // Calculate checksum
    i = 0; cs = 0;
    while (rx_str[i])
    {
        cs ^= rx_str[i];
        i++;
    }
    // If checksum is correct - return 1
    if (cs == rx_cs)
        return 1;
    else
        return 0;
}

char *strtok_ex(char *buf, const char *delim)
{
    char *p;
    static char *secret_state;

    p = buf ? buf : secret_state;
    if (p == 0)
        return (0);

    /*
    * Do *not* skip (span) initial delimiters.  Find end of
    * non-delimiter "word" part, and if there is anything after
    * that make sure the next strtok(NULL) will pick up there.
    */
    secret_state = p + strcspn(p, delim);
    if (*secret_state)
        *secret_state++ = 0;
    else
        secret_state = 0;

    return (p);
}

usr_nmea_satellite * nmea_get_info(uint8_t index)
{
    return &satinfo_copy[index];
}
