#ifndef __SYSTEM_BASIS_H__
#define __SYSTEM_BASIS_H__

#define SYSTEM_CLOCK MCK

#define DELAY_MS(ms) do {						\
			unsigned int i = ms * (SYSTEM_CLOCK / 1000 / 5);\
			while (i--) {					\
				asm volatile (				\
						"NOP\n\t"		\
						"NOP\n\t"		\
						"NOP\n\t"		\
						"NOP\n\t"		\
					);				\
			}						\
		} while (0);

#define DELAY_US(us) do {							\
			unsigned int i = us * (SYSTEM_CLOCK / 1000000 / 5);	\
			while (i--) {						\
				asm volatile (					\
						"NOP\n\t"			\
						"NOP\n\t"			\
						"NOP\n\t"			\
						"NOP\n\t"			\
					);					\
			}							\
		} while (0);

#endif

