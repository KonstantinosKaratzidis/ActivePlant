#include <avr/interrupt.h>
#include <avr/io.h>
#include "config.h"
#include "dev.h"
#include "utils.h"

uint32_t millis_to_ticks(uint32_t millis){
	uint32_t ticks = millis / MILLIS_PER_TICK;
	if(ticks * MILLIS_PER_TICK < millis){
		ticks++;
	}
	return ticks;
}

static volatile bool interrupts_enabled = false;

void enable_interrupts(){
	cli();
	interrupts_enabled = true;
	sei();
}

void disable_interrupts(){
	cli();
	interrupts_enabled = false;
}

void lock(){
	cli();
}

void unlock(){
	cli(); // to get atomic reading of interrupts_enabled
	if(interrupts_enabled)
		sei();
}

void idle(){
	SMCR = 0x01;
}
