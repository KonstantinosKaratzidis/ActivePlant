#ifndef ACTIVEPLANT_CONTROL_H
#define ACTIVEPLANT_CONTROL_H

typedef enum {
	STATE_IDLE, // the minimum time period between two waterings has not passed
	STATE_INSPECTING, // the plant can water itself if needed
	STATE_PUMP_ON, // pump is open
	STATE_STABILIZATION // just after watering, waiting for a certain period
		                  // so that moisture reading can stabilize
} watering_state_t;

/* Return the control loops current state
 */
watering_state_t control_current_state();

// a very basic, one directional state machine
// the conceptual states are:
//  * inspection interval passes -> moisture level can be manipulated
//  * wait until watering is needed -> open the pump
//  * close the pump and wait for moisture level to stabilize
//  * repeat
//
// This function needs to be called periodically
void control_periodic();

// initializes the control's state and periodic intervals
void control_init();

#endif // ACTIVEPLANT_CONTROL_H
