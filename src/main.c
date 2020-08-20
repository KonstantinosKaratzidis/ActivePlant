#include <stdio.h>
#include <ctype.h>
#include <util/delay.h>

#include "config.h"
#include "control.h"
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
	comm_init(settings_get_comm_address());
	control_init();
	interval_set(
		INTERVAL_LOG,
		millis_to_ticks(settings_get_log_interval() * 1000)
	);
}

void log_status(){}

int main(){
	init();
	enable_interrupts();
	dprintf("initialization done\r\n");

	packet_t msg;
	while(1){ 
		if(comm_get_packet(&msg)){
			process_request(&msg);
		}

		if(interval_has_passed(INTERVAL_LOG))
			log_status();

		control_periodic();

		idle();
	}
}
