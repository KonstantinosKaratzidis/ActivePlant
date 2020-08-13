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

// callback executed upon receiving a packet
typedef void (*comm_req_cb)(const packet_t*);

void invalid_op_cb(const packet_t *req){
	comm_send_error(req, COMM_ERR_INVAL_OP);
}

comm_req_cb comm_callbacks[REG_NUM_REGS] = {
	[REG_PING] = NULL,
	[REG_PONG] = invalid_op_cb,
	[REG_WATER_LEVEL] = NULL,
	[REG_MOISTURE] = NULL,
	[REG_MOISTURE_WANTED] = NULL,
	[REG_WATER_INTERVAL] = NULL,
	[REG_LOG_INTERVAL] = NULL,
};

void process_request(const packet_t *req){
	const uint8_t *req_magic = req->magic;

	if( // if no match -> not a package for us
			(req_magic[0] != COMM_MAGIC0) ||
			(req_magic[1] != COMM_MAGIC1) ||
			(req_magic[2] != COMM_MAGIC2) ||
			(req_magic[3] != COMM_MAGIC3)
	){
		dprintf("invalid magic\r\n");
		return;
	}

	// if not addressed to us
	if(req->dst != COMM_ADDRESS){
		dprintf("packet not addressed to us\r\n");
		return;
	}

	// if invalid reg
	if(req->reg >= REG_NUM_REGS){
		dprintf("invalid register: %d\r\n", req->reg);
		return comm_send_error(req, COMM_ERR_INVAL_REG);
	}

	comm_req_cb callback = comm_callbacks[req->reg];
	if(callback == NULL){
		dprintf("callback undefined\r\n");
		return comm_send_error(req, COMM_ERR_UNKNOWN);
	}
	callback(req);
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
