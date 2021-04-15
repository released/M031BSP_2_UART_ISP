/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>

#include "NuMicro.h"

#include	"project_config.h"

#include "targetdev.h"
#include "uart_transfer.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void ISP_Init(void)
{
    /* Checking if flash page size matches with target chip's */
    if( (GET_CHIP_SERIES_NUM == CHIP_SERIES_NUM_I) || (GET_CHIP_SERIES_NUM == CHIP_SERIES_NUM_G) )
    {
        if(FMC_FLASH_PAGE_SIZE != 2048)
        {
            /* FMC_FLASH_PAGE_SIZE is different from target device */
            /* Please enable the compiler option PAGE_SIZE_2048 in fmc.h */
            while(SYS->PDID);
        }
    }
    else
    {
        if(FMC_FLASH_PAGE_SIZE != 512)
        {
            /* FMC_FLASH_PAGE_SIZE is different from target device */
            /* Please disable the compiler option PAGE_SIZE_2048 in fmc.h */
            while(SYS->PDID);
        }
    }

    CLK->AHBCLK |= CLK_AHBCLK_ISPCKEN_Msk;
    FMC->ISPCTL |= (FMC_ISPCTL_ISPEN_Msk | FMC_ISPCTL_APUEN_Msk);
    g_apromSize = GetApromSize();
    GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);
    SysTick->LOAD = 300000 * CyclesPerUs;
    SysTick->VAL   = (0x00);
    SysTick->CTRL = SysTick->CTRL | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC clock (Internal RC 48MHz) */
    CLK->PWRCTL |= CLK_PWRCTL_HIRCEN_Msk;

    /* Wait for HIRC clock ready */
    while (!(CLK->STATUS & CLK_STATUS_HIRCSTB_Msk));

    /* Select HCLK clock source as HIRC and HCLK clock divider as 1 */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk)) | CLK_CLKSEL0_HCLKSEL_HIRC;
    CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_HCLKDIV_Msk)) | CLK_CLKDIV0_HCLK(1);
    /* Set PLL to Power-down mode */
    CLK->PLLCTL |= CLK_PLLCTL_PD_Msk;

    CLK->APBCLK0 |= CLK_APBCLK0_UART1CKEN_Msk;
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART1SEL_Msk)) | CLK_CLKSEL1_UART1SEL_HIRC;
    CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_UART1DIV_Msk)) | CLK_CLKDIV0_UART1(1);

    CLK->APBCLK0 |= CLK_APBCLK0_UART2CKEN_Msk;
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL3_UART2SEL_Msk)) | CLK_CLKSEL3_UART2SEL_HIRC;
    CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV4_UART2DIV_Msk)) | CLK_CLKDIV4_UART2(1);

	// ISP UART1 , PA9 , PA8
    SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA8MFP_Msk | SYS_GPA_MFPH_PA9MFP_Msk)) |
                    (SYS_GPA_MFPH_PA8MFP_UART1_RXD | SYS_GPA_MFPH_PA9MFP_UART1_TXD);

	// ISP UART2
	#if 1	// PB5 , PB4
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk)) |
                    (SYS_GPB_MFPL_PB4MFP_UART2_RXD | SYS_GPB_MFPL_PB5MFP_UART2_TXD);
	#else	// PF4 , PF5
    SYS->GPF_MFPL = (SYS->GPF_MFPL & ~(SYS_GPF_MFPL_PF5MFP_Msk | SYS_GPF_MFPL_PF4MFP_Msk)) |
                    (SYS_GPF_MFPL_PF5MFP_UART2_RXD | SYS_GPF_MFPL_PF4MFP_UART2_TXD);				
	#endif


}

/*
 * This is a template project for M031 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

int main()
{
    SYS_Init();

	UART1_Init();
	UART2_Init();
	
	ISP_Init();	

    /* Got no where to go, just loop forever */
    while(1)
    {
        if ((bufhead1 >= 4) || (bUartDataReady1 == TRUE))
        {
            uint32_t lcmd1;
            lcmd1 = inpw(uart_rcvbuf1);

            if (lcmd1 == CMD_CONNECT)
            {
                goto _ISP;
            }
            else
            {
                bUartDataReady1 = FALSE;
                bufhead1 = 0;
            }
        }

        if ((bufhead2 >= 4) || (bUartDataReady2 == TRUE))
        {
            uint32_t lcmd2;
            lcmd2 = inpw(uart_rcvbuf2);

            if (lcmd2 == CMD_CONNECT)
            {
                goto _ISP;
            }
            else
            {
                bUartDataReady2 = FALSE;
                bufhead2 = 0;
            }
        }		

        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
        {
            goto _APROM;
        }
    }

_ISP:

    while (1)
    {
        if (bUartDataReady1 == TRUE)
        {
            bUartDataReady1 = FALSE;
            ParseCmd(uart_rcvbuf1, MAX_PKT_SIZE);
            PutStringTargetUART(UART1);
        }

        if (bUartDataReady2 == TRUE)
        {
            bUartDataReady2 = FALSE;
            ParseCmd(uart_rcvbuf2, MAX_PKT_SIZE);
            PutStringTargetUART(UART2);
        }		

    }

_APROM:
    SYS->RSTSTS = (SYS_RSTSTS_PORF_Msk | SYS_RSTSTS_PINRF_Msk);
    FMC->ISPCTL &= ~(FMC_ISPCTL_ISPEN_Msk | FMC_ISPCTL_BS_Msk);
    SCB->AIRCR = (V6M_AIRCR_VECTKEY_DATA | V6M_AIRCR_SYSRESETREQ);

    /* Trap the CPU */
    while (1);
    
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
