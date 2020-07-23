#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "config.h"
#include "io.hpp"

void print(const char *msg){
	size_t len = strlen(msg);
	usart.write(msg, len);
}

void print(uint16_t num){
	char buf[12];
	sprintf(buf, "%d", num);
	print(buf);
}


void init_adc(){
	// set_reference
	ADMUX |= (1 << REFS0);

	// set_clk_prescale();
	ADCSRA |= 7;
	// enable();
	ADCSRA |= (1 << ADEN);
}

void set_channel(uint8_t chn){
	chn &= 0x0f;
	ADMUX &= 0xf0;
	ADMUX |= chn;
}

uint16_t adc_read(uint8_t channel){
	set_channel(channel);
	// start a conversion
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC))
		; // wait for conversion to complete
	return ADC;
}

void init(){
	//init_devices();
	usart.init();
	init_adc();
}

void send_samples(uint16_t first, uint16_t second){
	char buf[6];
	buf[0] = 0xff;
	buf[1] = 0xff;
	buf[2] = first & 0xff;
	buf[3] = first >> 8;
	buf[4] = second & 0xff;
	buf[5] = second >> 8;
	usart.write(buf, 6);
}


int main(){
	init();
	sei();
	while(1){
		uint16_t channel_1 = adc_read(0);
		uint16_t channel_2 = adc_read(1);
		send_samples(channel_1, channel_2);
		_delay_ms(1000);
	}
	return 0;
}
