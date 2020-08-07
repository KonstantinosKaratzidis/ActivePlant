#include "config.h"
#include "moisture.h"
#include "dev.h"

#define FILTER_SZ MOISTURE_FILTER_BUF_SZ

// we implement a very simple low filter for the adc values
// we calculate the average over FILTER_SZ samples
// the way it is done, it adds a bit of latency, but the moisture values
// do not change so dramatically, so this is not an issue

// variables used for the sampling filter
static uint16_t samples[FILTER_SZ];
static uint32_t sum = 0;
static int index = 0;

// hook to adc and update filter state
void adc_new_sample_hook(uint16_t sample){
	sum -= samples[index];
	sum += sample;
	samples[index] = sample;

	if(++index == FILTER_SZ){
		index = 0;
	}
}

uint16_t moisture_read_raw(){
	return sum / FILTER_SZ;
}
