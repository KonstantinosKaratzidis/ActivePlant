#ifndef PRD_CONFIG_H
#define PRD_CONFIG_H
#include <avr/io.h>

#ifndef F_CPU
#error "F_CPU not defined (needed for clock rate calculations)"
#endif

#define DEBUG 1
#define DEBUG_PRINT_BUF_SZ 64

#ifndef DEBUG_BAUD
#define DEBUG_BAUD 9600UL
#endif

// for the test pin (e.g. can use to time events)
#define DEBUG_PIN_DDR  DDRD
#define DEBUG_PIN_PORT PORTD
#define DEBUG_PIN_PIN  PD7

// for the bitbanged debug uart
#define DEBUG_UART_DDR  DDRD
#define DEBUG_UART_PORT PORTD
#define DEBUG_UART_PIN  PD6

#define PUMP_DDR DDRB
#define PUMP_PORT PORTB
#define PUMP_PIN PB0

#define USART_TX_BUF_SZ 128
#define USART_RX_BUF_SZ 128

#define ADC_BUF_SZ 256

#endif // PRD_CONFIG_H
