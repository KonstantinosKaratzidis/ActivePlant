#include "config.h"
#include "control.h"
#include "periodic.h"
#include "moisture.h"
#include "settings.h"
#include "utils.h"

static watering_state_t state;

void control_init(){
	state = CONTROL_INITIAL_STATE;
	interval_set(
		INTERVAL_WATER,
		millis_to_ticks(settings_get_water_interval() * 1000)
	);
}

void control_periodic(){
	switch(state){
	case STATE_IDLE:
	{
		if(interval_has_passed(INTERVAL_WATER))
			state = STATE_INSPECTING;
	}
		break;
	case STATE_INSPECTING:
	{
		// TODO: read tank water level here
		if(moisture_read() < settings_get_moisture_wanted()){
			pump_open();
			state = STATE_PUMP_ON;
			interval_set(INTERVAL_PUMP, millis_to_ticks(PUMP_ON_MS));
		}
	}
		break;
	case STATE_PUMP_ON:
	{
		if(interval_has_passed(INTERVAL_PUMP)){
			pump_close();
			state = STATE_STABILIZATION;
			interval_set(
				INTERVAL_WATER_STABILIZE,
				millis_to_ticks(MOISTURE_STABILIZE_WAIT_MS)
			);
		}
	}
		break;
	case STATE_STABILIZATION:
	{
		if(interval_has_passed(INTERVAL_WATER_STABILIZE)){
			interval_set(
				INTERVAL_WATER,
				millis_to_ticks(settings_get_water_interval() * 1000)
			);
			state = STATE_IDLE;
		}
	}
		break;
	}
}
