#ifndef ACTIVEPLANT_MOISTURE_H
#define ACTIVEPLANT_MOISTURE_H
#include <stdint.h>

// raw (but filtered) sensor value
uint16_t moisture_read_raw();

// normalized (linearly increasing) value
uint16_t moisture_read_norm();

#endif
