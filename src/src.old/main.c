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

#define PERIOD (TICKS_PER_SECOND / 4)

int main(){
	init();
	enable_interrupts();
	interval_set(INTERVAL_LOG, millis_to_ticks(2000));

	char buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	for(int i = 0; i < 10; i++){
		buf[i] += 10;
	}

	while(1){
		packet_t msg;
		if(comm_get_packet(&msg)){
			if(msg.dst != COMM_ADDRESS){
				dprintf("dst not correct\r\n");
				dprintf("was %02x\r\n", msg.dst);
			} else {
				dprintf("received packet\r\n");
				uart_write(buf, 10);
			}
		}
		idle();
	}
}
