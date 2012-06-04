#include "cpu-init.h"
#include "debug-uart.h"
#include <stm32f10x_map.h>
#include <gpio.h>

void cpu_init(void) {
	volatile unsigned int StartUpCounter = 0;
	volatile unsigned int HSEStatus = 0;
	
	/* System Init */
	RCC->CR |= (unsigned int)0x00000001;
	RCC->CFGR &= (unsigned int)0xF8FF0000;
	RCC->CR &= (unsigned int)0xFEF6FFFF;
	RCC->CR &= (unsigned int)0xFFFBFFFF;
	RCC->CFGR &= (unsigned int)0xFF80FFFF;
	RCC->CIR = 0x00000000;

	RCC->CR |= RCC_CR_HSEON;
	do {
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		++StartUpCounter;
	} while ((HSEStatus == 0) && (StartUpCounter != 0x0500));
	
	if ((RCC->CR & RCC_CR_HSERDY) != RESET)
		HSEStatus = (unsigned int)0x01;
	else
		HSEStatus = (unsigned int)0x00;
	if (HSEStatus == (unsigned int)0x01) {
		/*!< Enable Prefetch Buffer */
		FLASH->ACR |= FLASH_ACR_PRFTBE;

		/*!< Flash 2 wait state */
		FLASH->ACR &= (unsigned int)((unsigned int)~FLASH_ACR_LATENCY);
		FLASH->ACR |= (unsigned int)FLASH_ACR_LATENCY_2;    

		/*!< HCLK = SYSCLK */
		RCC->CFGR |= (unsigned int)RCC_CFGR_HPRE_DIV1;

		/*!< PCLK2 = HCLK */
		RCC->CFGR |= (unsigned int)RCC_CFGR_PPRE2_DIV1;

		/*!< PCLK1 = HCLK */
		RCC->CFGR |= (unsigned int)RCC_CFGR_PPRE1_DIV2;

		/*!< PLLCLK = 8MHz * 9 = 72 MHz */
		RCC->CFGR &= (unsigned int)((unsigned int)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
		RCC->CFGR |= (unsigned int)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);

		/*!< Enable PLL */
		RCC->CR |= RCC_CR_PLLON;

		/*!< Wait till PLL is ready */
		while((RCC->CR & RCC_CR_PLLRDY) == 0) ;

		/*!< Select PLL as system clock source */
		RCC->CFGR &= (unsigned int)((unsigned int)~(RCC_CFGR_SW));
		RCC->CFGR |= (unsigned int)RCC_CFGR_SW_PLL;    

		/*!< Wait till PLL is used as system clock source */
		while ((RCC->CFGR & (unsigned int)RCC_CFGR_SWS) != (unsigned int)0x08) ;
	} else {
		while (1) ;
	}

	// setup uart
	dbg_setup_uart_default();
}

