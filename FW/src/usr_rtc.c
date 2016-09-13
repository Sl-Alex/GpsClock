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
* @file usr_rtc.c
* @brief RTC driver for built-in RTC module
* @details Built-in RTC module should use UTC time.
* Right before output time should be converted to local time.
* As for me - it is the easiest way to have correct time :)
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include "usr_rtc.h"

/// @brief Initialize built-in RTC module
void usr_rtc_init(void)
{
    //taskENTER_CRITICAL();
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY - 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    if (BKP_ReadBackupRegister(BKP_DR1) == USR_RTC_CONFIGURED)
    {
        /* Enable PWR and BKP clocks */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);
        /* Enable LSE */
        RCC_LSEConfig(RCC_LSE_ON);
        /* Wait till LSE is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
        {}
        /* Select LSE as RTC Clock Source */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        /* Enable RTC Clock */
        RCC_RTCCLKCmd(ENABLE);

        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();
        /* Enable the RTC Second */
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        //taskEXIT_CRITICAL();
        return;
    }

    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Reset Backup Domain */
    BKP_DeInit();

    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}

    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    BKP_WriteBackupRegister(BKP_DR1, USR_RTC_CONFIGURED);
    //taskEXIT_CRITICAL();
}

/// @brief Set time in built-in RTC module
/// @param[in] year - full year (2014, for example)
/// @param[in] month - Month number (1..12)
/// @param[in] day - Day of month (1..31)
/// @param[in] hour - Hour of day (UTC)
/// @param[in] minute - Minute (UTC)
/// @param[in] second - Second (UTC)
void usr_rtc_set(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second)
{
    taskENTER_CRITICAL();
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Change the current time */
    RTC_SetCounter(hour*3600 + minute*60 + second);
    rtc_time.year = year;
    rtc_time.month = month;
    rtc_time.day = day;
    rtc_time.hour = hour;
    rtc_time.minute = minute;
    rtc_time.second = second;
    taskEXIT_CRITICAL();
}

/// @brief Reads time from built-in RTC module and updates @ref rtc_time structure
/// @param[in] rtc_value - Value from built-in RTC module
void usr_rtc_convert(uint32_t rtc_value)
{
    static uint32_t prev_hour = 0;
    uint32_t rest = 0;

    rtc_time.hour = (rtc_value / 3600) % 24;
    rest = rtc_value % 3600;
    rtc_time.minute = rest / 60;
    rtc_time.second = rest % 60;
    if (rtc_time.year == 0) return;
    if ((rtc_time.hour == 0) && (prev_hour == 23))
    {
        rtc_time.day++;
        if (rtc_time.day > usr_time_utils_get_days_of_month(rtc_time.year, rtc_time.month))
        {
            rtc_time.day = 1;
            rtc_time.month++;
            if (rtc_time.month > 12)
            {
                rtc_time.month = 1;
                rtc_time.year++;
            }
        }
    }
    prev_hour = rtc_time.hour;
}

/// @brief Converts @ref rtc_time to @ref loc_time using UTC+2 (EET) time zone
void usr_rtc_get_local_time(void)
{
    usr_time_utils_get_local_datetime(&rtc_time, &loc_time, 2 + usr_time_utils_check_summer_time(&rtc_time));
    loc_time.minute = rtc_time.minute;
    loc_time.second = rtc_time.second;
}
