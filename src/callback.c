#include <stdint.h>
#include <stdlib.h>
#include "config.h"
#include "callback.h"
#include "moisture.h"
#include "settings.h"
#include "periodic.h"
#include "debug.h"

#define WARN_UNIMPLEMENTED() {dprintf("UNIMPLEMTED %s %d\r\n", __FILE__, __LINE__);} while(0)

#define FLAG_IS_READ(flags) (((flags) & COMM_FLAG_WRITE) == 0)
#define FLAG_IS_WRITE(flags) ((flags) & COMM_FLAG_WRITE)

// helper macro, called inside callbacks for registers that do not accept
// write operations
#define deny_write(req) \
	do{\
		if(FLAG_IS_WRITE((req)->flags)){\
			return invalid_op_cb((req));\
		}\
	}while(0)\

// callback executed upon receiving a packet
typedef void (*comm_req_cb)(const packet_t*);

static inline uint16_t get_data(const packet_t *packet){
	return (packet->data_high << 8) | (packet->data_low);
}

static void invalid_op_cb(const packet_t *req){
	dprintf("invalid operation\r\n");
	comm_send_error(req, COMM_ERR_INVAL_OP);
}

static void ping_cb(const packet_t *req){
	deny_write(req);
	dprintf("got PING, send PONG\r\n");
	comm_send_packet(req->src, COMM_FLAG_RESP, REG_PING, 0);
}

static void water_level_cb(const packet_t *req){
	deny_write(req);
	WARN_UNIMPLEMENTED();
	dprintf("send water level: %d\r\n", 0);
	comm_send_ok(req, 0);
}

static void moisture_cb(const packet_t *req){
	deny_write(req);
	uint16_t moisture = moisture_read();
	dprintf("moisture: %d\r\n", moisture);
	comm_send_ok(req, moisture);
}

static void moisture_wanted_cb(const packet_t *req){
	uint16_t wanted = settings_get_moisture_wanted();
	dprintf("moisture wanted: %d\r\n", wanted);
	comm_send_ok(req, wanted);
}

// For multipart writes the low part needs to be sent first
// and then the high part. The write will have effect only after the
// high part has been written
// TODO: in the future, if there seems to be a need for more multipart
// values, the code will need to be refactored into more reusable units

static void water_interval_cb(const packet_t *req){
	static uint16_t low_part = 0;
	dprintf("water inteval cb\r\n");

	if(FLAG_IS_READ(req->flags)){
		uint16_t value;
		if(req->reg == REG_WATER_INTERVAL_LOW)
			value = settings_get_water_interval() & 0xffff;
		else
			value = (settings_get_water_interval() >> 16) & 0xffff;
		comm_send_ok(req, value);
	} else {
		if(req->reg == REG_WATER_INTERVAL_LOW){
			low_part = get_data(req);
			comm_send_ok(req, low_part);
		} else { // received the high part, change the interval
			uint32_t new_value = ((uint32_t)get_data(req) << 16) | low_part;
			
			// the underlying communication unreliable, which means we may receive
			// the same command more than 1 time. It is better to first check
			// if the new value is actually different and not a repeated command,
			// thus potentially avoiding an expensive eeprom update
			if(new_value != settings_get_water_interval()){
				settings_set_water_interval(new_value);
				interval_set( // this will also reset the interval
					INTERVAL_WATER,
					millis_to_ticks(new_value * 1000)
				);
			}
			comm_send_ok(req, get_data(req));
		}
	}
}

static void log_interval_cb(const packet_t *req){
	static uint16_t low_part = 0;
	dprintf("log inteval cb\r\n");

	if(FLAG_IS_READ(req->flags)){
		uint16_t value;
		if(req->reg == REG_LOG_INTERVAL_LOW)
			value = settings_get_log_interval() & 0xffff;
		else
			value = (settings_get_log_interval() >> 16) & 0xffff;
		comm_send_ok(req, value);
	} else {
		if(req->reg == REG_LOG_INTERVAL_LOW){
			low_part = get_data(req);
			comm_send_ok(req, low_part);
		} else { // received the high part, change the interval
			uint32_t new_value = ((uint32_t)get_data(req) << 16) | low_part;
			
			// the underlying communication unreliable, which means we may receive
			// the same command more than 1 time. It is better to first check
			// if the new value is actually different and not a repeated command,
			// thus potentially avoiding an expensive eeprom update
			if(new_value != settings_get_log_interval()){
				settings_set_log_interval(new_value);
				interval_set( // this will also reset the interval
					INTERVAL_LOG,
					millis_to_ticks(new_value * 1000)
				);
			}
			comm_send_ok(req, get_data(req));
		}
	}
}

comm_req_cb comm_callbacks[REG_NUM_REGS] = {
	[REG_PING] = ping_cb,
	[REG_WATER_LEVEL] = water_level_cb,
	[REG_MOISTURE] = moisture_cb,
	[REG_MOISTURE_WANTED] = moisture_wanted_cb,
	[REG_WATER_INTERVAL_LOW] = water_interval_cb,
	[REG_WATER_INTERVAL_HIGH] = water_interval_cb,
	[REG_LOG_INTERVAL_LOW] = log_interval_cb,
	[REG_LOG_INTERVAL_HIGH] = log_interval_cb,
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


	#if COMM_LOG_PACKET
	dprintf("recv: ");
	for(int i = 0; i < 10; i++)
		dprintf("0x%02x ", ((uint8_t *) req)[i]);
	dprintf("\r\n");
	dprintf("src=0x%02x (%d), ", req->src, req->src);
	dprintf("dst=0x%02x (%d), ", req->dst, req->dst);
	dprintf("flags=0x%02x (%d), ", req->flags, req->flags);
	dprintf("reg=0x%02x (%d), ", req->reg, req->reg);
	dprintf("data=0x%04x (%d)\r\n", req->data_high << 8 | req->data_low, req->data_high << 8 | req->data_low);
	#endif

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
	// callback should not be null as we already checked that it is
	// in the correct range, but just to be certain we check it
	// (otherwise callback could end up being some interrupt vector)
	if(callback == NULL){
		dprintf("callback undefined\r\n");
		return comm_send_error(req, COMM_ERR_UNKNOWN);
	}
	callback(req);
}
