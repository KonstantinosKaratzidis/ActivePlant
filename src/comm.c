#include <string.h>
#include "config.h"
#include "comm.h"
#include "dev.h"
#include "utils.h"
#include "debug.h"

#if !defined(COMM_QUEUE_ENTRIES)
#error "COMM_QUEUE_ENTRIES not defined"
#endif

// buffer for receiving a single buffer
static uint8_t recv_buf[10];
static uint8_t recv_buf_idx = 0;

// a fifo for storing incoming packets
// implemented as ring buffer
static packet_t queue[COMM_QUEUE_ENTRIES];
static int queue_write_idx = 0; // next writable index
static int queue_read_idx = 0;  // next readable index
static int queue_num_free = COMM_QUEUE_ENTRIES;

void comm_init(uint8_t addr){
	// empty
}

// this function is called inside an interrupt
// no need to lock and unlock
void uart_recv_hook(uint8_t byte){
	recv_buf[recv_buf_idx++] = byte;

	if(recv_buf_idx < 10){
		return;
	}

	// else we have a full packet
	
	// try and put the new packet in the queue
	if(queue_num_free == 0){ // no space in queue
		comm_queue_full_hook();
	} else { // put in queue
		memcpy(&queue[queue_write_idx], recv_buf, 10);
		queue_write_idx = (queue_write_idx + 1) % COMM_QUEUE_ENTRIES;
		queue_num_free--;
	}
	recv_buf_idx = 0;
}

bool comm_get_packet(packet_t *packet){
	bool success;
	lock();


	if(queue_num_free == 10){ // queue is empty
		success = false;
		goto done;
	}

	memcpy(packet, &queue[queue_read_idx], 10);

	// update queue index and number of free items in buffers
	queue_read_idx = (queue_read_idx + 1) % COMM_QUEUE_ENTRIES;
	queue_num_free++;
	success = true;

done:
	unlock();
	return success;
}

void _FUNC_HOOK comm_queue_full_hook(){
	dprintf("receive queue is full\r\n");
}

void comm_send_packet(
		uint8_t dst,
		uint8_t flags,
		uint8_t reg,
		uint16_t data)
{
	packet_t packet = STATIC_PACKET_INIT(COMM_ADDRESS, dst, flags, reg, data);
	uart_write((const char *) &packet, 10);
}

void comm_send_ok(const packet_t *request, uint16_t data){
	comm_send_packet(request->src, (1 << COMM_FLAG_RESP), request->reg, data);
}

void comm_send_error(const packet_t *request, uint16_t errno){
	comm_send_packet(
			request->src, (1 << COMM_FLAG_RESP) | (1 << COMM_FLAG_ERR),
			request->reg, errno);
}
