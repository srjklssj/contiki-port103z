#include "spi-arch.h"

// Notice: there are two slave devices -- enc28j60 and cc2500
// enc28j60 -- spi1
// cc2500   -- spi2
void spi_init(void) {
	// ----------------- SPI1 ---------------------- //
	// enable spi1 and gpio clocks
	RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN);
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	// lan-cs -- PA4
	GPIO_CONF_OUTPUT_PORT(A, 4, PUSH_PULL, 50);

	// spi1 pins: sck  -- PA5  miso -- PA6  mosi -- PA7
	GPIO_CONF_OUTPUT_PORT(A, 5, ALT_PUSH_PULL, 50);
	GPIO_CONF_INPUT_PORT(A, 6, PU_PD);
	GPIO_CONF_OUTPUT_PORT(A, 7, ALT_PUSH_PULL, 50);

	// spi1 configuration
	SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_0 | SPI_CR1_SSM | SPI_CR1_SSI;
	SPI1->CRCPR = (unsigned short)0x0007;
	// ---------------------------------------------- //
	
	// ----------------- SPI2 ----------------------- //
	// enable spi2 and gpio clocks
	RCC->APB2ENR |= (RCC_APB2ENR_IOPBEN);
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

	// rf-cs -- PB12
	GPIO_CONF_OUTPUT_PORT(B, 12, PUSH_PULL, 50);
	
	// spi2 pins: sck -- PB13 miso -- PB14 mosi -- PB15
	GPIO_CONF_OUTPUT_PORT(B, 13, ALT_PUSH_PULL, 50);
	GPIO_CONF_INPUT_PORT(B, 14, PU_PD);
	GPIO_CONF_OUTPUT_PORT(B, 15, ALT_PUSH_PULL, 50);

	// spi2 configuration
	SPI2->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_SSM | SPI_CR1_SSI;
	SPI2->CRCPR = (unsigned short)0x0007;
	// ---------------------------------------------- //

	// Enable SPI
	SPI1->CR1 |= SPI_CR1_SPE;
	SPI2->CR1 |= SPI_CR1_SPE;
}

