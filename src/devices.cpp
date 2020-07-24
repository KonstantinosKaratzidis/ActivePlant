#include "io.hpp"
#include <avr/interrupt.h>

Pump pump;
Moisture moisture;
Usart usart;
Timer timer;

ISR(ADC_vect){
	moisture.update(ADC);
	ADCSRA |= (1 << ADSC);
}

ISR(TIMER0_COMPA_vect){
	timer._tick();
}

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
