#ifndef ACTIVEPLANT_COMM_H
#define ACTIVEPLANT_COMM_H
#include <stdint.h>
#include <stdbool.h>
#include "utils.h"

/* Functions that handle the communication though the ble-to-serial
 * converter.
 */

// TODO: get an actual magic number
#define COMM_MAGIC0 0xde
#define COMM_MAGIC1 0xad
#define COMM_MAGIC2 0xde
#define COMM_MAGIC3 0xed

#define COMM_FLAG_ERR (1 << 0)
#define COMM_FLAG_RESP (1 << 1)
// from master to slave
// 0 for read, 1 for write
#define COMM_FLAG_RW (1 << 2)

#define COMM_ERR_UNKNOWN (1 << 0) // unknown error
#define COMM_ERR_NO_REG (1 << 1) // register does not exist
#define COMM_ERR_INVAL_OP (1 << 2) // invalid operation (eg setting moisture)
#define COMM_ERR_INVAL_REG (1 << 2) // invalid register (eg doesnt exist)

typedef enum {
	REG_PING = 0,
	REG_PONG,
	REG_WATER_LEVEL,
	REG_MOISTURE,
	REG_MOISTURE_WANTED,
	REG_WATER_INTERVAL,
	REG_LOG_INTERVAL,
	REG_NUM_REGS
} reg_t;

typedef struct {
	uint8_t magic[4]; 
	uint8_t src;
	uint8_t dst;

	// one byte for flags
	uint8_t flags;

	uint8_t reg;

	// little endian
	uint8_t data_low;
	uint8_t data_high;
} __attribute__((packed)) packet_t;

#define STATIC_PACKET_INIT(src, dst, flags, reg, data) \
	{ \
		{COMM_MAGIC0, COMM_MAGIC1, COMM_MAGIC2, COMM_MAGIC3}, \
		(src), \
		(dst), \
		(flags), \
		(reg), \
		((uint8_t)(data & 0xff)), \
		((uint8_t)((data >> 8) & 0xff)) \
	}

// just in case, will not compile if sizeof(packet_t) != 10
typedef char
ERROR_packet_should_be_ten_bytes[(sizeof(packet_t) == 10) ? 1 : -1];

void comm_init(uint8_t own_address);

// If a packet is in the queue, returns true and sets packet to
// the packet contents. Otherwise it returns false.
bool comm_get_packet(packet_t *packet);

// hook function for when there is no space in the receive queue
void comm_queue_full_hook();

// send a packet
void comm_send_packet(
		uint8_t dst,
		uint8_t flags,
		uint8_t reg,
		uint16_t data);

// helper methods for responding to requests
void comm_send_ok(const packet_t *request, uint16_t data);
void comm_send_error(const packet_t *request, uint16_t errno);

#endif // ACTIVEPLANT_COMM_H
