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

#endif // ACTIVEPLANT_CONFIG_H
