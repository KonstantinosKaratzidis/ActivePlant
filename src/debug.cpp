#include <stdarg.h>
#include <stdio.h>
#include <util/delay_basic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "config.h"
#include "debug.hpp"

constexpr uint16_t delay_cycles_per_bit = F_CPU / DEBUG_BAUD / 4;

void DebugUart::init(){
	DEBUG_UART_DDR |= (1 << DEBUG_UART_PIN);
	DEBUG_UART_PORT |= (1 << DEBUG_UART_PIN);
}

void DebugUart::write_byte(char byte){
	cli();
	// start bit
	DEBUG_UART_PORT &= ~(1 << DEBUG_UART_PIN);
	_delay_loop_2(delay_cycles_per_bit);

	for(int i = 0; i < 8; i++){
		uint8_t bit = byte & (1 << i);
		if(bit){
			DEBUG_UART_PORT |= (1 << DEBUG_UART_PIN);
		}
		else{
			DEBUG_UART_PORT &= ~(1 << DEBUG_UART_PIN);
		}
		_delay_loop_2(delay_cycles_per_bit);
	}

	// stop bit
	DEBUG_UART_PORT |= (1 << DEBUG_UART_PIN);
	_delay_loop_2(delay_cycles_per_bit);
	_delay_loop_2(delay_cycles_per_bit);
	sei();
}

void DebugUart::write(const char *buf, int size){
	for(int i = 0; i < size; i++)
		write_byte(buf[i]);
}

void DebugUart::printf(const char *fmt, ...){
	char buf[DEBUG_PRINT_BUF_SZ];
	va_list ap;
	va_start(ap, fmt);
	int written = vsnprintf(buf, DEBUG_PRINT_BUF_SZ, fmt, ap);
	write(buf, written);
	va_end(ap);
}

//
// TestPin
//

void TestPin::init(){
	DEBUG_PIN_DDR |= (1 << DEBUG_PIN_PIN);
}

void TestPin::on(){
	DEBUG_PIN_PORT |= (1 << DEBUG_PIN_PIN);
}

void TestPin::off(){
	DEBUG_PIN_PORT &= ~(1 << DEBUG_PIN_PIN);
}

void TestPin::toggle(){
	DEBUG_PIN_PORT ^= (1 << DEBUG_PIN_PIN);
}
