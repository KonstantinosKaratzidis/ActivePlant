#include <stdint.h>

#include "config.h"
#include "periodic.h"
#include "dev.h"
#include "utils.h"
#include "debug.h"

typedef struct {
	uint32_t period;
	uint32_t ticks;
	bool can_read;
} counter_state_t;

counter_state_t counter_states[_INTERVAL_NUM];


bool interval_has_passed(interval_id_t id){
	if(id >= _INTERVAL_NUM){
		dprintf("non existent id passed ot interval_has_passed\r\n");
		return false;
	}

	counter_state_t *state = &counter_states[id];
	bool ret = false;

	lock();   // enter critical section
	if(state->can_read){
		ret = true;
		state->can_read = false;
	}
	unlock(); // leave critical section

	return ret;
}

void timer_tick_hook(){
	for(int i = 0; i < _INTERVAL_NUM; i++){
		counter_state_t *state = &counter_states[i];

		// check for zero first, otherwise it could wrap around on
		// on the first call or if period is set to zero
		if(state->ticks == 0){
			state->ticks = state->period;
			state->can_read = true;
		} else {
			state->ticks--;
		}
	}
}

void interval_set(interval_id_t id, uint32_t ticks){
	if(id >= _INTERVAL_NUM){
		return;
	}
	counter_state_t *state = &counter_states[id];

	lock();   // enter critical section
	state->period = ticks;
	state->ticks = ticks;
	state->can_read = false;
	unlock(); // leave critical section
}
