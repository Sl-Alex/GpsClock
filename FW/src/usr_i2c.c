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
* @file usr_i2c.c
* @brief i2c functionality
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include "usr_i2c.h"
#include "FreeRTOS.h"
#include "task.h"

///@brief Actual timeout can be one tick longer
#define I2C_TIMEOUT         7
#define I2C_RETRANSMISSION  3

void usr_i2c_init(void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    // Enable peripheral clock for I2C1, I2C2, GPIOB and AFIO
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1  | RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);//

    // Configure I2C1 pins: PB6->SCL  and PB7->SDA
    // Configure I2C2 pins: PB10->SCL and PB11->SDA
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6  | GPIO_Pin_7 |
                                   GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Deinit I2C1 and I2C2
    I2C_DeInit(I2C1);
    I2C_DeInit(I2C2);

    // Prepare I2C configuration for both i2c channels
    /// @todo (Sl-Alex#1#) Update i2c clock to 400k
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 1;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;  /* 100kHz */

    // Configure I2C1
    I2C_Cmd(I2C1, ENABLE);
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    // Configure I2C2
    I2C_Cmd(I2C2, ENABLE);
    I2C_Init(I2C2, &I2C_InitStructure);
    I2C_AcknowledgeConfig(I2C2, ENABLE);
}

uint32_t usr_i2c_stop(I2C_TypeDef * interface)
{
    TickType_t xLastWakeTime;
    // Generate STOP condition
    I2C_GenerateSTOP(interface, ENABLE);
    xLastWakeTime = xTaskGetTickCount ();
    while(xTaskGetTickCount() <= (xLastWakeTime + I2C_TIMEOUT))
    {
        if (I2C_GetFlagStatus(interface, I2C_FLAG_BUSY) == RESET)
        {
            return 1;
        }
    }
    // Failed, send STOP condition
    return 0;
}

uint32_t usr_i2c_start(I2C_TypeDef * interface)
{
    TickType_t xLastWakeTime;
    //vTaskDelay(1);
    I2C_GenerateSTART(interface, ENABLE);
    xLastWakeTime = xTaskGetTickCount ();
    while(xTaskGetTickCount() <= (xLastWakeTime + I2C_TIMEOUT))
    {
        if (I2C_CheckEvent(interface, I2C_EVENT_MASTER_MODE_SELECT) != 0)
        {
            return 1;
        }
    }
    // Failed, send STOP condition
    usr_i2c_stop(interface);
    return 0;
}

uint32_t usr_i2c_send_addr_rd(I2C_TypeDef * interface, uint8_t addr)
{
    TickType_t xLastWakeTime;
    /* Send dev_addr for read */
    I2C_Send7bitAddress(interface, addr << 1, I2C_Direction_Receiver);
    xLastWakeTime = xTaskGetTickCount ();
    while(xTaskGetTickCount() <= (xLastWakeTime + I2C_TIMEOUT))
    {
        if (I2C_CheckEvent(interface, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != 0)
        {
            return 1;
        }
    }
    // Failed, send STOP condition
    usr_i2c_stop(interface);
    return 0;
}

uint32_t usr_i2c_send_addr_wr(I2C_TypeDef * interface, uint8_t addr)
{
    TickType_t xLastWakeTime;
    /* Send dev_addr for read */
    I2C_Send7bitAddress(interface, addr << 1, I2C_Direction_Transmitter);
    xLastWakeTime = xTaskGetTickCount ();
    while(xTaskGetTickCount() <= (xLastWakeTime + I2C_TIMEOUT))
    {
        if (I2C_CheckEvent(interface, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != 0)
        {
            return 1;
        }
    }
    // Failed, send STOP condition
    usr_i2c_stop(interface);
    return 0;
}

uint32_t usr_i2c_writedata(I2C_TypeDef * interface, uint8_t * data, uint8_t datasize)
{
    TickType_t xLastWakeTime;
    uint8_t tx_done;

    for (uint8_t i = 0; i < datasize; i++)
    {
        // Write byte
        I2C_SendData(interface, data[i]);
        xLastWakeTime = xTaskGetTickCount ();
        tx_done = 0;
        while(xTaskGetTickCount() <= (xLastWakeTime + I2C_TIMEOUT))
        {
            if (I2C_CheckEvent(interface, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != 0)
            {
                tx_done = 1;
                break;
            }
        }

        if (tx_done != 0)
            continue;

        /* Send STOP condition */
        usr_i2c_stop(interface);
        return 0;
    }
    return datasize;
}

uint32_t usr_i2c_transfer_once(I2C_TypeDef * interface, uint8_t addr, uint8_t * wr, uint8_t wr_size, uint8_t * rd, uint8_t rd_size)
{
    TickType_t xLastWakeTime;
    uint8_t rx_done = 0;

    if (wr_size > 0)
    {
        if (usr_i2c_start(interface) == 0)
            return 0;
        if (usr_i2c_send_addr_wr(interface, addr) == 0)
            return 0;
        if (usr_i2c_writedata(interface,wr,wr_size) == 0)
            return 0;
    }
    if (rd_size > 0)
    {
        if (usr_i2c_start(interface) == 0)
            return 0;
        if (usr_i2c_send_addr_rd(interface, addr) == 0)
            return 0;
        // Read rd_size bytes
        I2C_NACKPositionConfig(interface,I2C_NACKPosition_Current);

        for (uint8_t i = 0; i < rd_size; i++)
        {
            // If it is the last byte
            if (i == (rd_size - 1))
            {
                I2C_GenerateSTOP(interface, ENABLE);
                xLastWakeTime = xTaskGetTickCount ();
                while(xTaskGetTickCount() <= (xLastWakeTime + I2C_TIMEOUT))
                {
                    if (I2C_CheckEvent(interface, I2C_EVENT_MASTER_BYTE_RECEIVED) != 0)
                    {
                        rd[i] = I2C_ReceiveData(interface);
                        I2C_AcknowledgeConfig(interface, ENABLE);
                        return 1;
                    }
                }
                I2C_AcknowledgeConfig(interface, ENABLE);
                return 0;
            }
            xLastWakeTime = xTaskGetTickCount ();
            rx_done = 0;
            while(xTaskGetTickCount() <= (xLastWakeTime + I2C_TIMEOUT))
            {
                if (I2C_CheckEvent(interface, I2C_EVENT_MASTER_BYTE_RECEIVED) != 0)
                {
                    if ((i+2) >= rd_size)
                    {
                        I2C_AcknowledgeConfig(interface, DISABLE);
                    }
                    rd[i] = I2C_ReceiveData(interface);
                    rx_done = 1;
                    break;
                }
            }
            if (rx_done == 0){
                I2C_GenerateSTOP(interface, ENABLE);
                I2C_AcknowledgeConfig(interface, ENABLE);
                return 0;
            }

        }
    }
    if ((rd_size != 0) || (wr_size != 0))
    {
        usr_i2c_stop(interface);
        //vTaskDelay(1);
        I2C_AcknowledgeConfig(interface, ENABLE);
    }
    return 1;
//    1. Если txsize>0
//1.1 Передаём start.
//1.2 Передаём addr + W
//1.3 Передаём txsize байт начиная с адреса txdata
//2. Если rxsize>0
//2.1 Передаём start.
//2.2 Передаём addr + R
//1.3 Принимаем rxsize байт начиная с адреса rxdata
//3. Если (rxsize != 0) или (txsize != 0) передаём стоп, иначе возвращаем 0.
//В случае ошибки на любом этапе передаётся стоп и возвращается 0.
//В случае корректной работы возвращается 1.
}

uint32_t usr_i2c_transfer_once_2(I2C_TypeDef * interface, uint8_t addr, uint8_t * wr, uint8_t wr_size, uint8_t * wr2, uint8_t wr2_size)
{
    if (wr_size > 0)
    {
        if (usr_i2c_start(interface) == 0)
            return 0;
        if (usr_i2c_send_addr_wr(interface, addr) == 0)
            return 0;
        if (usr_i2c_writedata(interface,wr,wr_size) == 0)
            return 0;
    }
    if (wr2_size > 0)
    {
        if (usr_i2c_writedata(interface,wr2,wr2_size) == 0)
            return 0;
    }
    if ((wr_size != 0) || (wr2_size != 0))
    {
        usr_i2c_stop(interface);
        //vTaskDelay(1);
        I2C_AcknowledgeConfig(interface, ENABLE);
    }
    return 1;
}

void usr_i2c_reset(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    // Deinitialize I2C hardware
    I2C_DeInit(I2C1);
    I2C_DeInit(I2C2);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6  | GPIO_Pin_7 |
                                   GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Set SDA bits to 1
    GPIO_SetBits(GPIOB,GPIO_Pin_7);
    GPIO_SetBits(GPIOB,GPIO_Pin_11);

    // Set SCL bits to 1
    GPIO_SetBits(GPIOB,GPIO_Pin_6);
    GPIO_SetBits(GPIOB,GPIO_Pin_10);

    // Clock out 9 SCL pulses
    vTaskDelay(1);
    for (uint8_t i = 0; i < 9; i++)
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_6 | GPIO_Pin_10);
        vTaskDelay(1);
        GPIO_SetBits(GPIOB,GPIO_Pin_6 | GPIO_Pin_10);
        vTaskDelay(1);
    }

    usr_i2c_init();
}

uint32_t usr_i2c_transfer(I2C_TypeDef * interface, uint8_t addr, uint8_t * wr, uint8_t wr_size, uint8_t * rd, uint8_t rd_size)
{
    for (uint8_t i = 0; i < I2C_RETRANSMISSION; i++)
    {
        if (usr_i2c_transfer_once(interface,addr,wr,wr_size,rd,rd_size) != 0)
        {
            return 1;
        }
        else
        {
            usr_i2c_reset();
        }
    }
    return 0;
}

uint32_t usr_i2c_transfer_2(I2C_TypeDef * interface, uint8_t addr, uint8_t * wr, uint8_t wr_size, uint8_t * wr2, uint8_t wr2_size)
{
    for (uint8_t i = 0; i < I2C_RETRANSMISSION; i++)
    {
        if (usr_i2c_transfer_once_2(interface,addr,wr,wr_size,wr2,wr2_size) != 0)
        {
            return 1;
        }
        else
        {
            usr_i2c_reset();
        }
    }
    return 0;
}
