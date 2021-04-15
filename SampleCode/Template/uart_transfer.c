/**************************************************************************//**
 * @file     uart_transfer.c
 * @version  V1.00
 * $Date: 14/11/17 5:36p $
 * @brief    General UART ISP slave Sample file
 *
 * @note
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

/*!<Includes */
#include <string.h>
#include "NuMicro.h"
#include "uart_transfer.h"

#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t uart_rcvbuf1[MAX_PKT_SIZE] = {0};
uint8_t uart_rcvbuf2[MAX_PKT_SIZE] = {0};

#else
__attribute__((aligned(4))) uint8_t uart_rcvbuf1[MAX_PKT_SIZE] = {0};
__attribute__((aligned(4))) uint8_t uart_rcvbuf2[MAX_PKT_SIZE] = {0};

#endif

uint8_t volatile bUartDataReady1 = 0;
uint8_t volatile bufhead1 = 0;

uint8_t volatile bUartDataReady2 = 0;
uint8_t volatile bufhead2 = 0;
/* please check "targetdev.h" for chip specifc define option */

/*---------------------------------------------------------------------------------------------------------*/
/* INTSTS to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/

void UART02_IRQHandler(void)
{
    /*----- Determine interrupt source -----*/
    uint32_t u32IntSrc = UART2->INTSTS;

    if (u32IntSrc & 0x11)   /*RDA FIFO interrupt & RDA timeout interrupt*/
    {
        while (((UART2->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0) && (bufhead2 < MAX_PKT_SIZE))      /*RX fifo not empty*/
        {
            uart_rcvbuf2[bufhead2++] = UART2->DAT;
        }
    }

    if (bufhead2 == MAX_PKT_SIZE)
    {
        bUartDataReady2 = TRUE;
        bufhead2 = 0;
    }
    else if (u32IntSrc & 0x10)
    {
        bufhead2 = 0;
    }
}

void UART2_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Select UART function mode */
    UART2->FUNCSEL = ((UART2->FUNCSEL & (~UART_FUNCSEL_FUNCSEL_Msk)) | UART_FUNCSEL_MODE);
    /* Set UART line configuration */
    UART2->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
    /* Set UART Rx and RTS trigger level */
    UART2->FIFO = UART_FIFO_RFITL_14BYTES | UART_FIFO_RTSTRGLV_14BYTES;
    /* Set UART baud rate */
    UART2->BAUD = (UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200));
    /* Set time-out interrupt comparator */
    UART2->TOUT = (UART2->TOUT & ~UART_TOUT_TOIC_Msk) | (0x40);
    NVIC_SetPriority(UART02_IRQn, 2);
    NVIC_EnableIRQ(UART02_IRQn);
    /* 0x0811 */
    UART2->INTEN = (UART_INTEN_TOCNTEN_Msk | UART_INTEN_RXTOIEN_Msk | UART_INTEN_RDAIEN_Msk);
}

void UART13_IRQHandler(void)
{
    /*----- Determine interrupt source -----*/
    uint32_t u32IntSrc = UART1->INTSTS;

    if (u32IntSrc & 0x11)   /*RDA FIFO interrupt & RDA timeout interrupt*/
    {
        while (((UART1->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0) && (bufhead1 < MAX_PKT_SIZE))      /*RX fifo not empty*/
        {
            uart_rcvbuf1[bufhead1++] = UART1->DAT;
        }
    }

    if (bufhead1 == MAX_PKT_SIZE)
    {
        bUartDataReady1 = TRUE;
        bufhead1 = 0;
    }
    else if (u32IntSrc & 0x10)
    {
        bufhead1 = 0;
    }
}
#ifdef __ICCARM__
#pragma data_alignment=4
extern uint8_t response_buff[64];
#else
extern __attribute__((aligned(4))) uint8_t response_buff[64];
#endif 

void PutStringTargetUART(UART_T *uart)
{
    uint32_t i;

    for (i = 0; i < MAX_PKT_SIZE; i++)
    {
        while ((uart->FIFOSTS & UART_FIFOSTS_TXFULL_Msk));

        uart->DAT = response_buff[i];
    }
}

//void PutString1(void)
//{
//    uint32_t i;

//    for (i = 0; i < MAX_PKT_SIZE; i++)
//    {
//        while ((UART1->FIFOSTS & UART_FIFOSTS_TXFULL_Msk));

//        UART1->DAT = response_buff[i];
//    }
//}


void UART1_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Select UART function mode */
    UART1->FUNCSEL = ((UART1->FUNCSEL & (~UART_FUNCSEL_FUNCSEL_Msk)) | UART_FUNCSEL_MODE);
    /* Set UART line configuration */
    UART1->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
    /* Set UART Rx and RTS trigger level */
    UART1->FIFO = UART_FIFO_RFITL_14BYTES | UART_FIFO_RTSTRGLV_14BYTES;
    /* Set UART baud rate */
    UART1->BAUD = (UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200));
    /* Set time-out interrupt comparator */
    UART1->TOUT = (UART1->TOUT & ~UART_TOUT_TOIC_Msk) | (0x40);
    NVIC_SetPriority(UART13_IRQn, 2);
    NVIC_EnableIRQ(UART13_IRQn);
    /* 0x0811 */
    UART1->INTEN = (UART_INTEN_TOCNTEN_Msk | UART_INTEN_RXTOIEN_Msk | UART_INTEN_RDAIEN_Msk);
}

