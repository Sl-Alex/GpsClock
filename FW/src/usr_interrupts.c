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
* @file usr_interrupts.c
* @brief Interrupts
*
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include "usr_interrupts.h"

/// @brief USART interrupt handler (NMEA reception)
void USART2_IRQHandler()
{
    static uint8_t rx_byte;
	portBASE_TYPE xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = pdFALSE;

    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        rx_byte = USART_ReceiveData(USART2);
        xQueueSendFromISR(xQueueNMEA, &rx_byte, &xHigherPriorityTaskWoken );
    }
	/* Switch context if necessary */
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void RTC_IRQHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = pdFALSE;

    if (RTC_GetITStatus(RTC_IT_SEC) == SET)
    {
        /* Wait until last write operation on RTC registers has finished */
        //RTC_WaitForLastTask();
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_SEC);
        usr_rtc_convert(RTC_GetCounter());
        usr_rtc_get_local_time();
        xQueueSendFromISR(xQueueClock, &loc_time, &xHigherPriorityTaskWoken );
    }
    /* Switch context if necessary */
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void HardFault_Handler(void)
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n"
    );
}

void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress)
{
	/* These are volatile to try and prevent the compiler/linker optimising them
	away as the variables never actually get used.  If the debugger won't show the
	values of the variables, make them global my moving their declaration outside
	of this function. */
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* Link register. */
	volatile uint32_t pc; /* Program counter. */
	volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[0];
    r1 = pulFaultStackAddress[1];
    r2 = pulFaultStackAddress[2];
    r3 = pulFaultStackAddress[3];

    r12 = pulFaultStackAddress[4];
    lr = pulFaultStackAddress[5];
    pc = pulFaultStackAddress[6];
    psr = pulFaultStackAddress[7];

    /* When the following line is hit, the variables contain the register values. */
    while (1);

    /* These lines help prevent getting warnings from compiler about unused variables */
    r0 = r1 = r2 = r3 = r12 = lr = pc = psr = 0;
    r0++;
}
