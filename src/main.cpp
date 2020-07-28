#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "config.h"
#include "debug.hpp"
#include "io.hpp"

Pump pump;
Moisture moisture;
Usart usart;
Timer timer;

Device *devices[] = {
	&pump,
	&moisture,
	&usart,
	&timer,
	nullptr
};

void init_devices(){
	Device **device = devices;
	while(*device){
		(*device)->init();
		device++;
	}
}

ISR(ADC_vect){
	moisture.update(ADC);
	ADCSRA |= (1 << ADSC);
}

ISR(TIMER0_COMPA_vect){
	timer._tick();
}

void print(const char *msg){
	size_t len = strlen(msg);
	usart.write(msg, len);
}

void print(uint16_t num){
	char buf[12];
	sprintf(buf, "%d", num);
	print(buf);
}

char buf[4] = {(char) 0xff, (char) 0xff};
void send_sample(uint16_t sample){
	buf[2] = sample & 0xff;
	buf[3] = sample >> 8;
	usart.write(buf, 4);
}

void init(){
	init_devices();
	test_pin.init();
}

int main(){
	init();
	sei();
	while(1){
		if(timer.get_ticks() >= 100){
			timer.reset();
			send_sample(moisture.read());
		}
	}
	return 0;
}
