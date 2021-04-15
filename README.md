# M031BSP_2_UART_ISP
 M031BSP_2_UART_ISP


update @ 2021/04/15

1. initial UART 1 / UART 2　as ISP UART port 

	- UART 1 TX ,RX : PA9 , PA8
	
	- UART 2 TX ,RX : PB5 , PB4 /PF4 , PF5
	
2. base on M031 BSP ISP UART sample code , test on M031 EVM , with external UART bridge tool

3. below is ICP config setting with LDROM


![image](https://github.com/released/M031BSP_2_UART_ISP/blob/main/ICP_Config.jpg)


4. below is ICP tool , to programming LDROM and CONFIG

![image](https://github.com/released/M031BSP_2_UART_ISP/blob/main/ICP_tool.jpg)


5. below is ISP tool , to programming APROM (use test1.bin , test2.bin)

![image](https://github.com/released/M031BSP_2_UART_ISP/blob/main/ISP_tool.jpg)



