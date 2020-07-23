#include <avr/io.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include "config.h"
#include "io.hpp"

//
//                              class Pump
//

Pump::Pump(){}

void Pump::init(){
	PUMP_DDR |= (1 << PUMP_PIN);
}

void Pump::open(){
	PORTB |= (1 << PUMP_PIN);
	_state = State::open;
}

void Pump::close(){
	PORTB &= ~(1 << PUMP_PIN);
	_state = State::closed;
}

Pump::State Pump::get_state(){
	return _state;
}

void Pump::sleep(){
	close();
}

void Pump::wakeup(){
	this->init();
}

//
//                              class Moisture
//

Moisture::Moisture(){}

void Moisture::init(){
	set_reference();
	set_channel(0);
	set_clk_prescale();
	enable_interrupt();
	enable();
	start_conversion();
}

void Moisture::start_conversion(){
	ADCSRA |= (1 << ADSC);
}

uint16_t Moisture::read(){
	// start a conversion
	return samples_sum / ADC_BUF_SZ;
	// ADCSRA |= (1 << ADSC);
	// while(ADCSRA & (1 << ADSC))
	// 	; // wait for conversion to complete
	// return ADC;
}

void Moisture::update(uint16_t sample){
	pump.open();
	samples_sum -= samples[sample_index];
	samples_sum += sample;
	samples[sample_index] = sample;
	sample_index = (sample_index + 1) % ADC_BUF_SZ;
}

void Moisture::enable_interrupt(){
	ADCSRA |= (1 << ADIE);
}

void Moisture::set_reference(){
	// set reference to AVcc
	ADMUX |= (1 << REFS0);
}

void Moisture::set_channel(uint8_t chn){
	chn &= 0x0f;
	ADMUX &= 0xf0;
	ADMUX |= chn;
}

void Moisture::enable(){
	ADCSRA |= (1 << ADEN);
}

void Moisture::disable(){
	ADCSRA &= ~(1 << ADEN);
}

void Moisture::set_clk_prescale(){
#if F_CPU == 16000000UL
	ADCSRA |= 7;
#else
#error "TODO: code to handle different F_CPU"
#endif
}

void Moisture::sleep(){
	disable();
}

void Moisture::wakeup(){
	enable();
}

//
// Usart
//

static void rx_cb_nop(char c){}
static Usart::rx_cb_t registered_rx_cb = rx_cb_nop;

void call_rx_callback(char c){
	registered_rx_cb(c);
}

ISR(USART_RX_vect){
	call_rx_callback(UDR0);
}

Usart::Usart(){}

void Usart::init(){
	register_rx_cb(rx_cb_nop);
	// enable interrupt upon receive complete
  UCSR0B |= (1 << RXCIE0);
	
	// TODO: right now only 9600
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	// set character size to 8 bits
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	// enable trasmitter and receiver
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
}

void Usart::sleep(){}
void Usart::wakeup(){}

void Usart::write(const char *buf, size_t size){
	for(size_t i = 0; i < size; i++){
		// wait for data register empty
		while(!(UCSR0A & (1 << UDRE0)))
			;
		UDR0 = buf[i];
	}
}

void Usart::register_rx_cb(rx_cb_t rx_cb){
	registered_rx_cb = rx_cb;
}
