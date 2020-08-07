#ifndef ACTIVEPLANT_UTILS_H
#define ACTIVEPLANT_UTILS_H
#include <stdint.h>

// marks the function as a hook function that can be overwritten elsewhere
#define _FUNC_HOOK __attribute__((weak))

// A helper function to convert from milliseconds to number of ticks
// The tick amount is rounded upward, so the interval will actually be
// in the range [millis, millis + MILLIS_PER_TICK]
uint32_t millis_to_ticks(uint32_t millis);

void enable_interrupts();
void disable_interrupts();

// enter and leave a critical section
void lock();
void unlock();

// halts the cpus, wakes up on an interrupt
void idle();

#endif // ACTIVEPLANT_UTILS_H
