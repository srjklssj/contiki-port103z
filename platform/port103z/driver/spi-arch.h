#ifndef __SPI_ARCH_H__
#define __SPI_ARCH_H__
#include "gpio.h"
#include "stm32f10x_map.h"
#include <stdio.h>

void spi_init(void);

#define SPI1_RW(data)					\
	({						\
		while (0 == (SPI1->SR & SPI_SR_TXE));	\
		SPI1->DR = (data);			\
		while (0 == (SPI1->SR & SPI_SR_RXNE));	\
		SPI1->DR;				\
	 	})

#define SPI2_RW(data)					\
	({						\
	 	while (0 == (SPI2->SR & SPI_SR_TXE));	\
		SPI2->DR = (data);			\
		while (0 == (SPI2->SR & SPI_SR_RXNE));	\
		SPI2->DR;				\
	 	})

#endif
