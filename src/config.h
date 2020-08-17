#ifndef ACTIVEPLANT_CONFIG_H
#define ACTIVEPLANT_CONFIG_H
#include <avr/io.h>

#ifndef F_CPU
#error "F_CPU not defined (needed for clock rate calculations)"
#endif
 
#define BLE_BAUD (9600UL)
#define UART_BAUD BLE_BAUD

// pin definitions for pump
// (arduino pin D8)
#define PUMP_DDR DDRB
#define PUMP_PORT PORTB
#define PUMP_PIN PB0

// pin definitions for test
// can be used to measure timings
// (arduino pin D9)
#define TEST_DDR DDRB
#define TEST_PORT PORTB
#define TEST_PIN PB1

#define DEBUG 1
// pin definition for bitbanged uart
// (arduino pin D7)
#define DEBUG_DDR DDRD
#define DEBUG_PORT PORTD
#define DEBUG_PIN PD7
#define DEBUG_BUF_SZ 64 // buffer size for dprintf
#define DEBUG_BAUD 19200UL

#define MOISTURE_FILTER_BUF_SZ 256

#define EEPROM_SIZE 1000U

/* Communication */
// number of packets in the receiver queue
#define COMM_QUEUE_ENTRIES 10
#define COMM_ADDRESS 0xa0
// if set to 1, it gives a full packet log

#if DEBUG
#define COMM_LOG_PACKET 1
#else
#define COMM_LOG_PACKET 0
#endif

// the time period for which the pump is held open
#define PUMP_ON_MS 3000
// time time period to wait for the soil moisture reading
// to stabilize after having just watered the plant
#define MOISTURE_STABILIZE_WAIT_MS 300000UL
#define CONTROL_INITIAL_STATE STATE_INSPECTING

#endif // ACTIVEPLANT_CONFIG_H
