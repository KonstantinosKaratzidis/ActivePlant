#ifndef ACTIVEPLANT_DEV_H
#define ACTIVEPLANT_DEV_H
#include <stdint.h>
#include <stddef.h> // for size_t
#include <stdbool.h>
#include "utils.h"

/*
 * This file declares the functions used for accessing the low level
 * functionality of the chip.
 * These are the init functions for each 'device' and functions used to
 * use each device.
 *
 * Every device's init function needs to called before using the device to
 * ensure that it is in a known state before it is used
 *
 * NOTE: this is not a HAL, these are written with the application in mind
 *
 * NOTE: some devices have 'hooks' that let other code handle that device's
 * interrupts. Those functions are marked as weak symbols with
 * __attribute__((weak)). To register the hook define the function body
 * where it seems most appropriate.
 *
 */

typedef enum {
	LOW = 0,
	HIGH = 1
} pin_state_t;

                                                    /*--------------*/
                                                    /* === Pump === */
                                                    /*--------------*/
// Functions for controlling the pump

typedef enum pump_state_e {
	PUMP_CLOSED = LOW,
	PUMP_OPENED = HIGH
} pump_state_t;

void pump_init();
void pump_open();
void pump_close();

// TODO: not sure if needed, nice to have anyway
pump_state_t pump_state();

                                                /*-----------------------*/
                                                /* === Hardware uart === */
                                                /*-----------------------*/
// initializes the hardware uart with its mode set to
// 8N1, buad rate is determined by the configuration
// variable UART_BAUD
void uart_init();
void uart_writeb(uint8_t byte);
void uart_write(const char *buf, size_t len);

// hook function executed upon receive interrupt
void uart_recv_hook(uint8_t byte);

                                                          /*-------------*/
                                                          /* === ADC === */
                                                          /*-------------*/
/* A simple wrapper around the chip's built in ADC unit.
 * The adc is set to continuously sample the ADC's channel 0
 * and to call the function hook on every new sample obtained
 */
void adc_init();

// define this function to set a hook, which will get called
// every time there is a new sample
void adc_new_sample_hook(uint16_t sample);


                                                        /* --------------*/
                                                        /* === Timer === */
                                                        /* --------------*/
/* A simple way to register a function to be called once every tick
 * This way other code can implement and keep track of multiple
 * timers as needed. */
#define MILLIS_PER_TICK 10UL
#define TICKS_PER_SECOND 100

// Initializes the timer, timer starts ticking immediately
void timer_init();
void timer_start();
void timer_stop();

// define this function, it will called on each timer tick
void timer_tick_hook();

                                                    /* -----------------*/
                                                    /* === Test Pin === */
                                                    /* -----------------*/
void test_pin_init();
void test_pin_on();
void test_pin_off();
void test_pin_toggle();
pin_state_t test_pin_state();

#endif // ACTIVEPLANT_DEV_H
