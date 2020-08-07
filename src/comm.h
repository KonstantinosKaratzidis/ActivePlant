#ifndef ACTIVEPLANT_COMM_H
#define ACTIVEPLANT_COMM_H
#include <stdint.h>
#include <stdbool.h>
#include "utils.h"

/* Functions that handle the communication though the ble-to-serial
 * converter.
 */

typedef struct {
	uint8_t reserved[4];
	uint8_t src;
	uint8_t dst;

	uint8_t err:1; // if error, error code in data
	uint8_t resp:1; // set when responding to other device
	uint8_t rw_flag:1;

	uint8_t reg;

	uint8_t data_high;
	uint8_t data_low;
} __attribute__((packed)) packet_t;

typedef enum {
	REG_PING
} reg_t;

// just in case, will not compile if sizeof(packet) != 10
typedef char
ERROR_packet_should_ten_bytes[(sizeof(packet_t) == 10) ? 1 : -1];

void comm_init(uint8_t own_address);

// If a packet is in the queue, returns true and sets packet to
// the packet contents. Otherwise it returns false.
bool comm_get_packet(packet_t *packet);

// hook function for when there is no space in the receive queue
void _FUNC_HOOK comm_queue_full_hook();

#endif // ACTIVEPLANT_COMM_H
