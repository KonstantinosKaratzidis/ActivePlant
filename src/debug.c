#include <stdio.h>
#include <stdarg.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>

#include "config.h"
#include "debug.h"
#include "utils.h"

#if DEBUG

// _delay_loop_2 executes 4 cycles per count
static uint16_t delay_cycles_per_bit = F_CPU / DEBUG_BAUD / 4;

#define busy_wait() _delay_loop_2(delay_cycles_per_bit)

void debug_init(){
	DEBUG_PORT |= _BV(DEBUG_PIN); // idle state for uart is high
	DEBUG_DDR |= _BV(DEBUG_PIN);
}

void debug_write_byte(char byte){
	lock(); // enter critical section

	// start bit
	DEBUG_PORT &= ~(1 << DEBUG_PIN);
	busy_wait();

	for(int i = 0; i < 8; i++){
		uint8_t bit = byte & (1 << i);
		if(bit){
			DEBUG_PORT |= (1 << DEBUG_PIN);
		}
		else
			DEBUG_PORT &= ~(1 << DEBUG_PIN);
		busy_wait();
	}

	// stop bit
	DEBUG_PORT |= (1 << DEBUG_PIN);
	busy_wait();

	unlock(); // leave critical section
}

static void dwrite(const char *buf, uint16_t size){
	for(uint16_t i = 0; i < size; i++){
		debug_write_byte(buf[i]);
	}
}

void _dprintf(const char *fmt_addr, ...){
	static char buf[DEBUG_BUF_SZ];

	va_list ap;
	va_start(ap, fmt_addr);
	int written = vsnprintf_P(buf, DEBUG_BUF_SZ, fmt_addr, ap);
	dwrite(buf, written);

	va_end(ap);
}

#endif // DEBUG
