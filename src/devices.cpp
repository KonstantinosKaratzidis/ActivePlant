#include "io.hpp"
#include <avr/interrupt.h>

Pump pump;
Moisture moisture;
Usart usart;

ISR(ADC_vect){
	moisture.update(ADC);
	ADCSRA |= (1 << ADSC);
}


Device *devices[] = {
	&pump,
	&moisture,
	&usart,
	nullptr
};

void init_devices(){
	Device **device = devices;
	while(*device){
		(*device)->init();
		device++;
	}
}
