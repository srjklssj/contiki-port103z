#include <stm32f10x_map.h>
#include <nvic.h>
#include <sys/clock.h>
#include <sys/cc.h>
#include <sys/etimer.h>
#include "contiki-conf.h"

static volatile clock_time_t current_clock = 0;
static volatile unsigned long current_seconds = 0;
static unsigned int second_countdown = CLOCK_SECOND;

void SysTick_handler(void) __attribute__ ((interrupt));
void SysTick_handler(void) {
	(void)SysTick->CTRL;
	SCB->ICSR = SCB_ICSR_PENDSTCLR;
	current_clock++;
	if(etimer_pending() && etimer_next_expiration_time() <= current_clock) {
		etimer_request_poll();
	}
	if (--second_countdown == 0) {
		current_seconds++;
		second_countdown = CLOCK_SECOND;
	}
}

void clock_init() {
	NVIC_SET_SYSTICK_PRI(8);
	SysTick->LOAD = ((unsigned int)MCK) / 8 / CLOCK_SECOND;
	SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_TICKINT;
}

clock_time_t clock_time(void) {
	return current_clock;
}

unsigned long clock_seconds(void) {
	return current_seconds;
}

