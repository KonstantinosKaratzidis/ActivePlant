#ifndef ACTIVEPLANT_PERIODIC_H
#define ACTIVEPLANT_PERIODIC_H
#include "dev.h"
#include <stdint.h>

/* These functions are multiplexed on top of the
 * timer tick hook function, and they provide functionality in order
 * to keep track of periodic events. The idea is that if some code
 * needs to do something, it create's an 'interval' which can be tested
 * using the function 'interval_has_passed(intarval_id)'. This function
 * will return true if the interval has passed, and mark the interval as
 * read so that later calls will return false. When the interval passes
 * again, it will be marked as unread and calls to interval_has_passed
 * will return true.
 *
   Here is an illustration (for an interval will period 1 sec):
	 (interval_has_passed() is refered to as passed() to save space)
   T0  (0.7 sec)  (1.0 sec)  (1.2 sec) (1.3 sec) (2.0 sec) (3.1 sec)
   |      |          |           |         |        |         |
timer is  |       state ->       |         |     state ->     |
started   |       "unread"       |         |     "unread"     |
					|                      |         |                  |
					|                      |         |                  |
			passed()                passed()  passed()           passed()
			-> false                -> true   -> false           -> true
                              state ->                     state ->
                              "read"                       "unread"
 *
 * So the purpose of this code is to allow for a little bit more accurate
 * time keeping, but these are not made for very fast high precision events.
 */

// in order to define a new interval, all you have to do is
// add a new id, and call interval_set
typedef enum {
	INTERVAL_WATER, // used for watering the plant
	INTERVAL_WATER_STABILIZE,   // period to wait for moisture reading to stabilize after watering
	INTERVAL_PUMP, // used for controlling the pump
	INTERVAL_LOG,   // used for logging
	// ONLY ADD NEW INTERVALS OVER THIS LINE
	_INTERVAL_NUM   // used internally to allocate space for each interval
} interval_id_t;

// set the period for interval with given id
// this will also reset the internal state for the given interval,
// Which means that interval_has_passed will return true only after the
// amount of ticks specified.
void interval_set(interval_id_t id, uint32_t ticks);
bool interval_has_passed(interval_id_t id);

#endif // ACTIVEPLANT_PERIODIC_H
