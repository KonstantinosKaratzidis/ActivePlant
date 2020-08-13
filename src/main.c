#include <stdio.h>
#include <ctype.h>
#include <util/delay.h>

#include "config.h"
#include "dev.h"
#include "moisture.h"
#include "settings.h"
#include "debug.h"
#include "periodic.h"
#include "comm.h"
#include "utils.h"
#include "comm.h"
#include "callback.h"


void init(){
	settings_init();
	test_pin_init();
	pump_init();
	uart_init();
	adc_init();
	timer_init();
	debug_init();
	comm_init(COMM_ADDRESS);
}

void log_status(){}

#define PERIOD (TICKS_PER_SECOND / 4)

typedef enum {
	WAIT_INTERVAL,
	CAN_INSPECT,
	PUMP_ON, // has opened the pump
	AFTER_WATER // just after watering, waiting for a certain period
} watering_state_t;

void moisture_control(){
	static watering_state_t state = CAN_INSPECT;

	switch(state){
	case WAIT_INTERVAL:
	{
		if(interval_has_passed(INTERVAL_WATER))
			state = CAN_INSPECT;
	}
		break;
	case CAN_INSPECT:
	{
		// TODO: read tank water level here
		if(moisture_read() < settings_get_moisture_wanted()){
			pump_open();
			state = PUMP_ON;
			interval_set(INTERVAL_PUMP, millis_to_ticks(3000));
		}
	}
		break;
	case PUMP_ON:
	{
		if(interval_has_passed(INTERVAL_PUMP)){
			pump_close();
			state = AFTER_WATER;
			interval_set(INTERVAL_WATER_STABILIZE, millis_to_ticks(300 * 1000));
		}
	}
		break;
	case AFTER_WATER:
	{
		if(interval_has_passed(INTERVAL_WATER_STABILIZE)){
			interval_set(INTERVAL_WATER, millis_to_ticks(settings_get_water_interval() * 1000));
			state = WAIT_INTERVAL;
		}
	}
		break;
	}
}

int main(){
	init();
	interval_set(INTERVAL_LOG, millis_to_ticks(settings_get_log_interval() * 1000));
	interval_set(INTERVAL_WATER, millis_to_ticks(settings_get_water_interval() * 1000));
	enable_interrupts();

	packet_t msg;
	while(1){ 
		if(comm_get_packet(&msg)){
			process_request(&msg);
		}

		if(interval_has_passed(INTERVAL_LOG))
			log_status();

		moisture_control();

		idle();
	}
}
