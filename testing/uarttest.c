/******************************************************************************
* ADBeta (c)
******************************************************************************/
#include "lib_gpioctrl.h"
#include "ch32v003fun.h"
#include <stdio.h>


// TODO: predefine UART speeds

#define USART_HalfDuplex 0x00000008


int main()
{
	SystemInit();
	
	// Enable GPIOD and UART.
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;

	// Push-Pull, 10MHz Output, GPIO D5, with AutoFunction
	gpio_set_mode(GPIO_PD5, OUTPUT_10MHZ_PP | OUTPUT_PP_AF);
	gpio_set_mode(GPIO_PD6, INPUT_FLOATING);

	// 115200, 8n1.  Note if you don't specify a mode, UART remains off even when UE_Set.
	USART1->CTLR1 = USART_WordLength_8b | USART_Parity_No | USART_Mode_Tx;
	USART1->CTLR2 = USART_StopBits_1;
	USART1->CTLR3 = 0x00; // USART_HalfDuplex;

	USART1->BRR = UART_BRR;
	USART1->CTLR1 |= CTLR1_UE_Set;

/*
	char str[32] = "Hello World!!\r\n";
	while(1)
	{
		for(int i = 0; i < 16; i++){
	    	while( !(USART1->STATR & USART_FLAG_TC));
	    	USART1->DATAR = str[i];
		}

		Delay_Ms(100);
	}
*/
	while(1)
	{
		if(USART1->STATR & USART_FLAG_RXNE)
		{
			printf("data: ");
			char c = USART1->DATAR;
			printf("%c", c);
		}

	}
	
}
