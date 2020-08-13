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

#define WARN_UNIMPLEMENTED() {dprintf("UNIMPLEMTED %s %d\r\n", __FILE__, __LINE__);} while(0)

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

void ping_cb(const packet_t *req){
	dprintf("got ping, send pong\r\n");
	comm_send_packet(req->dst, (1 << COMM_FLAG_RESP), REG_PONG, 0);
}

void water_level_cb(const packet_t *req){
	WARN_UNIMPLEMENTED();
	dprintf("send water level: %d\r\n", 0);
	comm_send_ok(req, 0);
}

void moisture_cb(const packet_t *req){
	WARN_UNIMPLEMENTED();
	uint16_t moisture_raw = moisture_read_raw();
	dprintf("moisture: %d\r\n", moisture_raw);
	comm_send_ok(req, moisture_raw);
}

void moisture_wanted_cb(const packet_t *req){
	uint16_t wanted = settings_get_moisture_wanted();
	dprintf("moisture wanted: %d\r\n", wanted);
	comm_send_ok(req, wanted);
}

void water_interval_low_cb(const packet_t *req){
	uint16_t interval_low = settings_get_water_interval() & 0xffff;
	dprintf("water interval low: %u\r\n", interval_low);
	comm_send_ok(req, interval_low);
}

void water_interval_high_cb(const packet_t *req){
	uint16_t interval_high = (settings_get_water_interval() >> 16) & 0xffff;
	dprintf("water interval high: %u\r\n", interval_high);
	comm_send_ok(req, interval_high);
}

void log_interval_low_cb(const packet_t *req){
	uint16_t interval_low = settings_get_log_interval() & 0xffff;
	dprintf("log interval low: %u\r\n", interval_low);
	comm_send_ok(req, interval_low);
}

void log_interval_high_cb(const packet_t *req){
	uint16_t interval_high = (settings_get_log_interval() >> 16) & 0xffff;
	dprintf("log interval high: %u\r\n", interval_high);
	comm_send_ok(req, interval_high);
}

comm_req_cb comm_callbacks[REG_NUM_REGS] = {
	[REG_PING] = ping_cb,
	[REG_PONG] = invalid_op_cb,
	[REG_WATER_LEVEL] = water_level_cb,
	[REG_MOISTURE] = moisture_cb,
	[REG_MOISTURE_WANTED] = moisture_wanted_cb,
	[REG_WATER_INTERVAL_LOW] = water_interval_low_cb,
	[REG_WATER_INTERVAL_HIGH] = water_interval_high_cb,
	[REG_LOG_INTERVAL_LOW] = log_interval_low_cb,
	[REG_LOG_INTERVAL_HIGH] = log_interval_high_cb,
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
	process_request(NULL);

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
