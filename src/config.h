#ifndef PRD_CONFIG_H
#define PRD_CONFIG_H
#include <avr/io.h>

#ifndef F_CPU
#error "F_CPU not defined (needed for clock rate calculations)"
#endif

#define PUMP_DDR DDRB
#define PUMP_PORT PORTB
#define PUMP_PIN PB0

#define TEST_PIN_DDR DDRD
#define TEST_PIN_PORT PORTD
#define TEST_PIN PD7

#define USART_TX_BUF_SZ 128
#define USART_RX_BUF_SZ 128

#define ADC_BUF_SZ 256

#endif // PRD_CONFIG_H
