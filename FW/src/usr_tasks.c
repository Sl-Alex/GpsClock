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
* @file usr_tasks.c
* @brief FreeRTOS user tasks
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "usr_error.h"

#include "stm32f10x_iwdg.h"

#include "misc.h"
#include "usr_global.h"
#include "usr_rtc.h"

#include "usr_messages.h"
#include "usr_tasks.h"
#include "usr_i2c.h"
#include "usr_bh1750.h"
#include "usr_si7021.h"
#include "usr_bmp180.h"
#include "usr_matrix_hal.h"
#include "usr_nmea_parser.h"

#include "usr_fm24c04.h"

FRAM_LOG_T pressure_log;

/// @brief Tasks preparations
void vTasksInit(void)
{
    // Initialize NMEA USART
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef port;
    USART_InitTypeDef usart;

    // Enable necessary HW
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // USART transmitter (NMEA debug output)
    GPIO_StructInit(&port);
    port.GPIO_Mode = GPIO_Mode_AF_PP;
    port.GPIO_Pin = GPIO_Pin_2;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &port);

    // USART receiver (NMEA input from GPS)
    port.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    port.GPIO_Pin = GPIO_Pin_3;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &port);

    // Initialize USART
    USART_StructInit(&usart);
    usart.USART_BaudRate = NMEA_BAUDRATE;
    USART_Init(USART2, &usart);
    USART_Cmd(USART2, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    // Enable USART interrupt with correct priority
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY - 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

    // Initialize built-in RTC module
    usr_rtc_init();

    // Initialize i2c (display and all sensors are there)
    usr_i2c_init();
}


/// @brief NMEA decoding task
void vTaskNMEA(void *pvParameters)
{
    static uint8_t rx_byte;

    while(1)
    {
        // Blocking read from the queue
        xQueueReceive(xQueueNMEA, &rx_byte, portMAX_DELAY);
        usr_nmea_parse(rx_byte);
        if (usr_nmea_is_valid())
        {
            usr_nmea_valid_clear();
            if (usr_time_utils_need_sync(&gps_time, &rtc_time))
            {
                usr_rtc_set(gps_time.year, gps_time.month,  gps_time.day,
                            gps_time.hour, gps_time.minute, gps_time.second);
            }
        }
    }
}

void draw_pres_diagram(uint8_t x, uint8_t y, uint32_t height)
{
    uint32_t dia_min = UINT32_MAX;
    uint32_t dia_max = 0;
    uint32_t log_min, log_max;
    uint32_t diff;

    #define MIN_AUTOSCALE_DIFF  6

    // Find min and max for the whole diagram
    for (uint8_t i = 0; i < FRAM_LOG_SIZE; i++)
    {
        //log_max = usr_bmp180_get_pres_max(i);
        //log_min = usr_bmp180_get_pres_min(i);
        log_max = pressure_log.data[i].max;
        log_min = pressure_log.data[i].min;

        if ((log_max != 0) && (log_min != UINT32_MAX))
        {
            if (dia_max < log_max) dia_max = log_max;
            if (dia_min > log_min) dia_min = log_min;
        }
    }
    if ((dia_max == 0) || (dia_min == UINT32_MAX))
        return;

    diff = dia_max - dia_min;
    // Calculate top and bottom values for autoscale
    uint32_t top, bottom;
    if (diff > MIN_AUTOSCALE_DIFF)
    {
        top = dia_max;
        bottom = dia_min;
    }
    else
    {
        top = (dia_max + dia_min)/2 + MIN_AUTOSCALE_DIFF/2;
        bottom = (dia_max + dia_min)/2 - MIN_AUTOSCALE_DIFF/2;
    }

    // Draw the diagram
    for (uint8_t i = 0; i < FRAM_LOG_SIZE; i++)
    {
        uint32_t tmp_min;
        uint32_t tmp_max;

        //log_max = usr_bmp180_get_pres_max(i);
        //log_min = usr_bmp180_get_pres_min(i);
        log_max = pressure_log.data[i].max;
        log_min = pressure_log.data[i].min;
        if ((log_max == 0) || (log_min == UINT32_MAX))
            continue;

        tmp_min = (height-1) * (log_min - bottom) / (top - bottom);
        tmp_max = (height-1)* (log_max - bottom) / (top - bottom);

        for (uint8_t j = tmp_min; j <= tmp_max; j++)
        {
            usr_matrix_hal_set_pix(x + i, y + height - 1 - j, 1);
        }
    }
    if (diff > 99)
        diff = 99;
    if (diff > 9)
        usr_matrix_draw_digit_small(73,diff / 10);

    usr_matrix_draw_digit_small(77,diff % 10);
}

/// @brief Clock output task
void vTaskDisplay(void *pvParameters)
{
    static TIME_T clocktime;
    static uint8_t brightness;
    static uint32_t old_hum;
    static uint32_t old_temp;
    static uint32_t disp_hum;
    static uint32_t disp_temp;

    uint8_t from_watchdog = 0;

    static char tstr[5];

    // test the reset flags in order because the pin reset is always set.
    if  (RCC_GetFlagStatus(RCC_FLAG_IWDGRST)){
        from_watchdog = 1;
    }

    // The flags must be cleared manually after use
    RCC_ClearFlag();

    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescaler(IWDG_Prescaler_256);

    /* Set counter reload value to obtain 250ms IWDG TimeOut.
    Counter Reload Value = 250ms/IWDG counter clock period
                      = 250ms / (LSI/32)
                      = 0.25s / (LsiFreq/32)
                      = LsiFreq/(32 * 4)
                      = LsiFreq/128
    */
    IWDG_SetReload(40000/64);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();

    // Output "Search" message and a box for the progress bar
    if (from_watchdog == 0)
    {
        usr_matrix_hal_write_str(27,4,msg_search);
        usr_matrix_hal_draw_box(16,13,64,21);
        usr_matrix_hal_update(0);
    }

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    // Initialize sensors
    usr_si7021_init();  // Initialize humidity/temperature sensor
    usr_bh1750_init();  // Initialize brightness sensor
    usr_bmp180_init();  // Initialize pressure sensor

    // Loop until correct date/time from GPS are available
    uint8_t l = 17;
    uint8_t onoff = 1;
    while (1)
    {
        IWDG_ReloadCounter();

        // Check for the time message queue
        if (uxQueueMessagesWaiting(xQueueClock) != 0)
        {
            xQueueReceive(xQueueClock, &clocktime, (TickType_t) 0);
            // If date is correct then break out of the loop
            if (clocktime.year > 2000)
                break;
        }
        if (from_watchdog)
            continue;

        // Update progress bar
        for (uint8_t i = 14; i < 21; i++)
            usr_matrix_hal_set_pix(l,i,onoff);
        // Update LED display
        usr_matrix_hal_update(0);

        //usr_i2c_init();

        // Calculate progress bar coordinates and On/Off value
        l++;
        if (l == 64)
        {
            onoff = 1 - onoff;
            l = 17;
        }

    }

    // At this step we have the correct date/time, so we can load the pressure log from FRAM
    usr_fm24c04_load_data(&pressure_log);

    // Main task loop
    while (1)
    {
        /* Reload IWDG counter */
        IWDG_ReloadCounter();

        TIME_T rtc_time_copy;

        __disable_irq();
        rtc_time_copy = rtc_time;
        __enable_irq();

        usr_matrix_hal_clear(0x00);

        // Read brightness
        uint32_t br = usr_bh1750_read();
        // Update brightness only if it is correct
        if (br != UINT32_MAX)
        {
            // Reduce sensitivity
            br /= 128;
            // Limit maximum value
            if (br > 8) br = 8;
            // Update local brightness value
            brightness = br;
        }
        vTaskDelay(1);
        // Read pressure
        uint32_t pres = usr_bmp180_read_pressure();
//        if (((int32_t)pres - (int32_t)old_pres) < 2)
//        {
            if ((pres < 1000) && (pres > 500))
            {
                __disable_irq();
                usr_fm24c04_add_log(&pressure_log, pres, &rtc_time_copy);
                __enable_irq();
            }
//        }
//        old_pres = pres;

        vTaskDelay(1);
        // Read humidity
        uint32_t hum = usr_si7021_read_hum();
        if (((int32_t)hum - (int32_t)old_hum) > 1)
        {
            disp_hum = old_hum;
        }
        else
        {
            disp_hum = hum;
        }
        old_hum = hum;
        if (disp_hum > 99) disp_hum = 99;
        if (disp_hum == 0) disp_hum = hum;
        vTaskDelay(1);
        // Read temperature
        uint32_t temp = usr_si7021_read_temp();
        if (((int32_t)temp - (int32_t)old_temp) > 1)
        {
            disp_temp = old_temp;
        }
        else
        {
            disp_temp = temp;
        }
        old_temp = temp;
        if (disp_temp > 99) disp_temp = 99;
        if (disp_temp == 0) disp_temp = temp;

        // Start new humidity/temperature measurement
        usr_si7021_start();
        /// Output information to the LED display
        // Hours
        if (clocktime.hour>=10)
             usr_matrix_draw_digit_big( 0,clocktime.hour   / 10);
        usr_matrix_draw_digit_big(13,clocktime.hour   % 10);
        // Minutes
        usr_matrix_draw_digit_big(30,clocktime.minute / 10);
        usr_matrix_draw_digit_big(43,clocktime.minute % 10);
        // Seconds
        usr_matrix_draw_digit_medium(56,clocktime.second / 10);
        usr_matrix_draw_digit_medium(64,clocktime.second % 10);
        // Day of week
        uint8_t d = usr_time_utils_get_day_of_week(clocktime.year, clocktime.month, clocktime.day);
        usr_matrix_hal_write_str(0,17,msg_days[d]);
        // Day of month
        sprintf(tstr,"%2d",(int)clocktime.day);
        usr_matrix_hal_write_str(14,17,tstr);
        // Divider
        usr_matrix_hal_set_pix(26,23,1);
        // Month
        sprintf(tstr,"%02d",(int)clocktime.month);
        usr_matrix_hal_write_str(28,17,tstr);
        // Output dots between hours and minutes
        usr_matrix_hal_set_pix(26, 4, 1);
        usr_matrix_hal_set_pix(27, 4, 1);
        usr_matrix_hal_set_pix(26, 5, 1);
        usr_matrix_hal_set_pix(27, 5, 1);
        usr_matrix_hal_set_pix(26,10, 1);
        usr_matrix_hal_set_pix(27,10, 1);
        usr_matrix_hal_set_pix(26,11, 1);
        usr_matrix_hal_set_pix(27,11, 1);
        sprintf(tstr,"%02d%%",(int)disp_hum);
        usr_matrix_hal_write_str(44,17,tstr);
        sprintf(tstr,"%02d°",(int)disp_temp);
        usr_matrix_hal_write_str(64,17,tstr);
        // Pressure
        draw_pres_diagram(56,0,6);

        usr_nmea_satellite * sat;
        __disable_irq();
        uint8_t sat_count = 0;
        for (uint8_t i = 0; i < 8; i++)
        {
            sat = nmea_get_info(i);
            if (sat->id != NMEA_NOT_FOUND)
            {
                sat_count++;
            }
        }
        __enable_irq();
        if (sat_count > 8)
            sat_count = 8;
        for (uint8_t i = 0; i < sat_count; i++)
        {
            usr_matrix_hal_set_pix(72 + i, 15, 1);
        }
        // Output everything to the matrix
        usr_matrix_hal_update(brightness);
        // Blocking wait until new second
        xQueueReceive(xQueueClock, &clocktime, portMAX_DELAY);
        // Reset all stuck I2C communication (if any)
        usr_i2c_reset();
   }
}

/// @brief OS idle function
/// @note Do not block inside this function
void vApplicationIdleHook( void )
{

}

void vApplicationMallocFailedHook(void)
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	while (1);
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, signed char *pcTaskName)
{
	(void) pcTaskName;
	(void) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	while (1);
}
