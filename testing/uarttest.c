/******************************************************************************
* ADBeta (c)
******************************************************************************/
#include "lib_gpioctrl.h"
#include "ch32v003fun.h"
#include <stdio.h>

// Define some commonly used baus rates
// DIV = (HCLK / (16 * BAUD)) * 16 (HCLK is 48MHz)
typedef enum {

	UART_BAUD_921600 = 0x0034,    // 923076    
	UART_BAUD_460800 = 0x0068,    // 461538
	UART_BAUD_230400 = 0x00D3,    // 227488
	UART_BAUD_115200 = 0x01A1,    // 115107
	UART_BAUD_57600  = 0x0341,    // 57623
	UART_BAUD_38400  = 0x04E2,    // 38400
	UART_BAUD_19200  = 0x09C4,    // 19200
	UART_BAUD_9600   = 0x1388,    // 9600
	UART_BAUD_4800   = 0x2710,    // 4800
	UART_BAUD_2400   = 0x4E20,    // 2400
	UART_BAUD_1200   = 0x9C40,    // 1200
} UART_BAUDRATE_DEF;


int main()
{
	SystemInit();
	
	// Enable GPIOD and UART.
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;

	// Push-Pull, 10MHz Output, GPIO D5, with AutoFunction
	gpio_set_mode(GPIO_PD5, OUTPUT_10MHZ_PP | OUTPUT_PP_AF);
	gpio_set_mode(GPIO_PD6, INPUT_FLOATING);

	// 115200, 8n1.  Note if you don't specify a mode, UART remains off even when UE_Set.
	USART1->CTLR1 = USART_WordLength_8b | USART_Parity_No | USART_Mode_Tx | USART_Mode_Rx;
	USART1->CTLR2 = USART_StopBits_1;
	USART1->CTLR3 = 0x00;

	USART1->BRR = UART_BAUD_1200;
	USART1->CTLR1 |= CTLR1_UE_Set;


	while(1)
	{
		if( (USART1->STATR & USART_FLAG_RXNE) != 0)
		{
			char c = (char)USART1->DATAR;
			printf("%c", c);

			while( !(USART1->STATR & USART_FLAG_TC));
	    	USART1->DATAR = c;
		}

	}  
	
}
