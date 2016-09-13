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
*/

/**
* @mainpage
*
* @section brief_sec Brief description
*
* This project aims to create automatic clock, which uses GPS module to synchronize local time.
* Also it has automatic DST control
*
* @section license_sec License information
* The whole project except of parts from a 3<sup>rd</sup> party vendor (see below) is licensed
* under GPL v3 license. License information is located in "license.txt" file.
*
* Project uses the following source code from 3<sup>rd</sup> party vendors:
*   - CMSIS headers by ARM Limited (http://www.arm.com/)
*   - FreeRTOS operating system by Real Time Engineers Ltd (http://www.freertos.org/);
*   - CMSIS and peripheral library by STMicroelectronics (http://www.st.com/);
*
* For licensing information please refer to the appropriate source code files.
*   - CMSIS headers are located in "CMSIS" folder
*   - FreeRTOS distribution is located in "FreeRTOS" folder
*   - Peripheral library is located in "SPL" folder
*
* @section SEC_OTHER Changing firmware
*
* @note If you want to change time zone, you should modify @ref usr_rtc_get_local_time
* function. Currently time zone is set to UTC+2 (EET), DST is used.
*
* @note If you want to localize firmware, you should modify @ref usr_font variable.
* It contains complete one-byte code page, such as 1251 (Cyrillic). You are free to implement
* your own set of symbols. If you need pseudo-graphic - you can use, for example, symbols
* from 0x01 to 0x1F or (at you own risk) any other. Symbol 0x00 should not be used because
* all strings are null-terminated.<br />
* All text messages are contained in @ref usr_messages.c file. If you want to change these
* messages - please pay attention that this file must be saved in the same one-byte code page,
* as implemented in usr_font.c<br />
* As an additional good thing you should change <b>INPUT_ENCODING</b> parameter according to
* your code page, thus all Doxygen-generated documentation will be readable.
*
* @todo (Sl-Alex#99#) Write about manual brightness curve modification
*
* @copyright Oleksii Slabchenko, 2014
*/
#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "usr_time_utils.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "usr_global.h"

#include "usr_error.h"
#include "usr_ht16k33.h"
#include "usr_tasks.h"

int main(void)
{

    SystemCoreClockUpdate();
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

    // Queue for UART reception
    xQueueNMEA = xQueueCreate(32, sizeof(uint8_t));
    if (xQueueNMEA == 0)
        usr_error(__FILE__, __LINE__, "NMEA rx queue creation failed\n");
    // Queue for detected UTC time
    xQueueClock = xQueueCreate (10, sizeof(TIME_T));
    if (xQueueClock == 0)
        usr_error(__FILE__, __LINE__, "Clock queue creation failed\n");
    if(pdTRUE != xTaskCreate( vTaskNMEA, ( const char * ) "NMEA Decoder", configMINIMAL_STACK_SIZE*3, NULL, tskIDLE_PRIORITY + 1, NULL))
        usr_error(__FILE__, __LINE__, "NMEA task start failure\n");
    if(pdTRUE != xTaskCreate( vTaskDisplay, ( const char * ) "Clock", configMINIMAL_STACK_SIZE*3, NULL, tskIDLE_PRIORITY + 1, NULL))
        usr_error(__FILE__, __LINE__, "Clock task start failure\n");

    vTasksInit();

    vTaskStartScheduler();

    usr_error(__FILE__, __LINE__, "Scheduler start failure\n");
    while(1); // <- Should never be here
}
