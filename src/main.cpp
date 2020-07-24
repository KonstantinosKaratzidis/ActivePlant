#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "config.h"
#include "debug.hpp"
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

char buf[4] = {(char) 0xff, (char) 0xff};
void send_sample(uint16_t sample){
	buf[2] = sample >> 8;
	buf[3] = sample & 0xff;
	usart.write(buf, 4);
}

void init(){
	init_devices();
}

int main(){
	init();
	sei();
	test_pin.init();
	while(1){
		//send_sample(moisture.read());
		if(timer.get_ticks() >= 100){
			timer.reset();
			test_pin.toggle();
		}
	}
	return 0;
}
