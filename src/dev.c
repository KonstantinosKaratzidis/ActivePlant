#include <avr/io.h>
#include <avr/sfr_defs.h>
#include "config.h"
#include "dev.h"

#ifdef BAUD
# undef BAUD
#endif
# define BAUD UART_BAUD
#include <util/setbaud.h>
#include <avr/interrupt.h>


                                                    /* === Pump === */
void pump_init(){ PUMP_DDR |= _BV(PUMP_PIN); }
void pump_open(){ PUMP_PORT |= _BV(PUMP_PIN); }
void pump_close(){ PUMP_PORT &= ~_BV(PUMP_PIN); }
pump_state_t pump_state(){
	return bit_is_set(PUMP_PORT, PUMP_PIN) ? PUMP_OPENED : PUMP_CLOSED;
}

                                                /* === Hardware uart === */
// sets the baud to config value UART_BAUD
// it uses the helper macros from <util/setbaud.h>
static void setbaud(){
	// based on example from <util/setabaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	#if USE_2X
	UCSR0A |= (1 << U2X0);
	#else
	UCSR0A &= ~(1 << U2X0);
	#endif
}

void uart_init(){
	setbaud();

	// enable receive interrupt
	UCSR0B |= (1 << RXCIE0);

	// set frame size to 8 bits
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	// enable trasmitter and receiver
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
}

void uart_writeb(uint8_t byte){
	// wait for data register empty
	loop_until_bit_is_set(UCSR0A, UDRE0);
	// write byte to data register
	UDR0 = byte;
}

void uart_write(const char *buf, size_t len){
	for(size_t i = 0; i < len; i++)
		uart_writeb(buf[i]);
}

void _FUNC_HOOK uart_recv_hook(uint8_t byte){
	return;
}

ISR(USART_RX_vect){
	uart_recv_hook(UDR0);
}

                                                       /* === ADC === */
inline static void adc_start_conversion(){
	ADCSRA |= (1 << ADSC);
}

void adc_init(){
	// set reference to AREF
	ADMUX |= (0 << REFS1) | (0 << REFS0);
	
	// channel is set to 0 by default
	//ADMUX &= 0xf0;

	// set adc clock prescale
	#if F_CPU == 16000000UL
	ADCSRA |= 7;
	#else
	#error "TODO: code to handle different F_CPU"
	#endif

	// enable interupt on conversion
	ADCSRA |= (1 << ADIE);
	
	// enable the ADC
	ADCSRA |= (1 << ADEN);

	adc_start_conversion();
}

void _FUNC_HOOK adc_new_sample_hook(uint16_t sample){
	// do nothing
	// TODO: maybe show debugging message
}

ISR(ADC_vect){
	adc_new_sample_hook(ADC);
	adc_start_conversion();
}

                                                    /* === Timer === */
// The timer used is 8-bit timer/counter 0 (compare register A)

void timer_init(){
	// enable interrupt on output compare register
	TIMSK0 |= (1 << OCIE0A);
	OCR0A = 156;

	// set wave generation, clear timer on compare match
	TCCR0A |= (1 << WGM01) | (0 << WGM00);
	TCCR0B |= (0 << WGM02);
	timer_start();
}

void timer_stop(){
	// set prescaler to 0 (will stop timer)
	TCCR0B &= 0b11111000;
}

void timer_start(){
	// set prescaler to 1024 (will start timer)
	TCCR0B |= (1 << CS02) | (1 << CS00);
}

// the hook function for the timer's tick interrupt
void _FUNC_HOOK timer_tick_hook(){}

ISR(TIMER0_COMPA_vect){
	timer_tick_hook();
}

                                                    /* === Test Pin === */
void test_pin_init(){ TEST_DDR |= _BV(TEST_PIN); }
void test_pin_open(){ TEST_PORT |= _BV(TEST_PIN); }
void test_pin_close(){ TEST_PORT &= ~_BV(TEST_PIN); }
void test_pin_toggle(){ TEST_PORT ^= ~_BV(TEST_PIN); }
pin_state_t test_pin_state(){
	return bit_is_set(TEST_PORT, TEST_PIN) ? HIGH : LOW;
}
