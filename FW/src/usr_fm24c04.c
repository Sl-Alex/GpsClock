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
* @file usr_fm24c04.c
* @brief FRAM memory functions
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2015
*/

#include "usr_fm24c04.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usr_i2c.h"
#include "usr_time_utils.h"

#define FM24C04_ADDR 0x50
#define FM24C04_IFACE I2C2
#define ADDR1   0x000   // First block
#define ADDR2   0x100   // Second block

FRAM_LOG_T tmp_log;

uint32_t usr_fm24c04_read(uint32_t address, uint8_t * data, uint32_t datasize)
{
    uint8_t addr = address & 0xFF;
    address &= 0x1FF;

    return usr_i2c_transfer(FM24C04_IFACE, FM24C04_ADDR + ((address >> 8) & 0x01), &addr, 1, data, datasize);
}

uint32_t usr_fm24c04_write(uint32_t address, uint8_t * data, uint32_t datasize)
{
    uint8_t addr = address & 0xFF;
    address &= 0x1FF;

    return usr_i2c_transfer_2(FM24C04_IFACE, FM24C04_ADDR + ((address >> 8) & 0x01), &addr, 1, data, datasize);
}

uint32_t usr_fm24c04_get_free_address(void)
{
    static FRAM_LOG_HEADER_T header1;
    static FRAM_LOG_HEADER_T header2;

    usr_fm24c04_read(ADDR1, (uint8_t *)&header1, sizeof(FRAM_LOG_HEADER_T));
    usr_fm24c04_read(ADDR2, (uint8_t *)&header2, sizeof(FRAM_LOG_HEADER_T));

    if (header1.valid == 0)
        return ADDR1;
    if (header2.valid == 0)
        return ADDR2;

    int32_t comp = usr_time_utils_compare(&header2.last_item_end, &header1.last_item_end);
    if (comp > 0)
        return ADDR1;   // header1 location will be overwritten
    else
        return ADDR2;   // header2 location will be overwritten
}

/// @brief Double buffered write
void usr_fm24c04_write_data(FRAM_LOG_T * log)
{
    uint32_t addr;

    // Find outdated or invalid data to overwrite
    addr = usr_fm24c04_get_free_address();

    // Write the whole data with valid = 0
    log->header.valid = 0;
    usr_fm24c04_write(addr, (uint8_t *)log, sizeof(FRAM_LOG_T));

    // Set valid to 1 and update it in FRAM
    log->header.valid = 1;
    usr_fm24c04_write(addr, (uint8_t *)log, sizeof(log->header.valid));
}

uint32_t usr_fm24c04_get_valid_address(void)
{
    static FRAM_LOG_HEADER_T header1;
    static FRAM_LOG_HEADER_T header2;

    usr_fm24c04_read(ADDR1, (uint8_t *)&header1, sizeof(FRAM_LOG_HEADER_T));
    usr_fm24c04_read(ADDR2, (uint8_t *)&header2, sizeof(FRAM_LOG_HEADER_T));

    if ((header1.valid == 1) && (header2.valid == 1))
    {
        int32_t comp = usr_time_utils_compare(&header1.last_item_end, &header2.last_item_end);
        if (comp > 0)
            return ADDR1;   // header1 location is the most recent
        else
            return ADDR2;   // header2 location is the most recent
    }

    if (header1.valid == 1)
    {
        return ADDR1;   // header1 location is the most recent
    }

    if (header2.valid == 1)
    {
        return ADDR2;   // header2 location is the most recent
    }

    return UINT32_MAX;
}

/// @brief Load the most recent log from FRAM
/// @details log parameter must have correct last_item_end field
/// This time will be used to shift saved log to the current time
void usr_fm24c04_load_data(FRAM_LOG_T * plog)
{
    uint32_t addr;

    // Get the most recent valid address
    addr = usr_fm24c04_get_valid_address();

    // If there is no valid data, initialize with the default value
    if (addr == UINT32_MAX)
    {
        plog->header.valid = 0;
        plog->header.last_item_start.year = 0;
        plog->header.last_item_end.year = 0;
        for (uint8_t i = 0; i < FRAM_LOG_SIZE; i++)
        {
            plog->data[i].max = 0;
            plog->data[i].min = UINT32_MAX;
        }

        return;
    }

    // Return the most recent log
    usr_fm24c04_read(addr, (uint8_t *)plog, sizeof(FRAM_LOG_T));
}

/// @brief should be called once per second
void usr_fm24c04_add_log(FRAM_LOG_T * plog, uint32_t pres, TIME_T * putc_time)
{
    uint32_t hour_diff;
    static uint32_t pres_sum;
    static uint32_t pres_cnt;
    // Check the difference between last_item_start and the current time
    // If the difference is less than one hour then update current item
    // Otherwise shift the log and start new item

    hour_diff = usr_time_utils_hour_diff(&(plog->header.last_item_start), putc_time);

    // If the difference is smaller than the log size
    // Normal shift
    if ((hour_diff < FRAM_LOG_SIZE) && (hour_diff != 0))
    {
        // Shift the entire log
        for (uint8_t i = 0; i < (FRAM_LOG_SIZE - hour_diff); i++)
        {
            // Shift to the left
            plog->data[i] = plog->data[i+hour_diff];
        }
        // Erase the rest
        for (uint8_t i = (FRAM_LOG_SIZE - hour_diff); i < FRAM_LOG_SIZE; i++)
        {
            plog->data[i].min = UINT32_MAX;
            plog->data[i].max = 0;
        }

        // Add new data to the very last item and update the header
        plog->data[FRAM_LOG_SIZE - 1].max = pres;
        plog->data[FRAM_LOG_SIZE - 1].min = pres;
        plog->header.last_item_start = * putc_time;
        plog->header.last_item_end = * putc_time;

        pres_sum = pres;
        pres_cnt = 1;

        // Write data
        usr_fm24c04_write_data(plog);

        return;
    }

    // Difference is more than FRAM_LOG_SIZE
    // Just clear the old history and start new
    if (hour_diff >= FRAM_LOG_SIZE)
    {
        // Difference is bigger than FRAM_LOG_SIZE, reset the log
        plog->header.valid = 0;
        for (uint8_t i = 0; i < FRAM_LOG_SIZE; i++)
        {
            plog->data[i].max = 0;
            plog->data[i].min = UINT32_MAX;
        }
        plog->header.last_item_start = * putc_time;

        pres_sum = pres;
        pres_cnt = 1;
    }

    pres_sum += pres;
    pres_cnt++;

    pres = pres_sum / pres_cnt;

    // Update the last item
    plog->data[FRAM_LOG_SIZE - 1].max = pres;
    plog->data[FRAM_LOG_SIZE - 1].min = pres;
/*
    if (pres > plog->data[FRAM_LOG_SIZE - 1].max)
        plog->data[FRAM_LOG_SIZE - 1].max = pres;
    if (pres < plog->data[FRAM_LOG_SIZE - 1].min)
        plog->data[FRAM_LOG_SIZE - 1].min = pres;
*/
    plog->header.last_item_end = * putc_time;

    // Write data
    usr_fm24c04_write_data(plog);
}
