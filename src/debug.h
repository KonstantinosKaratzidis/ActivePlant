#ifndef ACTIVEPLANT_DEBUG_H
#define ACTIVEPLANT_DEBUG_H
#include <avr/pgmspace.h>
#include "config.h"

#if DEBUG

void debug_init();
// fmt_pgm_addr is an address in program address space
void _dprintf(const char *fmt_pgm_addr, ...);
#define dprintf(fmt, ...) _dprintf(PSTR(fmt), ##__VA_ARGS__)

#else
#define debug_init() {}
#define dprintf(...) {}
#endif

#endif // ACTIVEPLANT_DEBUG_H
