#include <stdint.h>
#include <avr/eeprom.h>
#include "comm.h"
#include "settings.h"
#include "config.h"

typedef struct {
	uint16_t moisture_wanted;
	uint16_t moisture_top;
	uint16_t moisture_bottom;
	uint32_t water_interval;
	uint32_t log_interval;
	comm_addr_t comm_address;
} __attribute__((packed)) settings_t;

/* Taken from user from
 * https://stackoverflow.com/questions/4079243/how-can-i-use-sizeof-in-a-preprocessor-macro#4079267
 * A trick to statically assert that the structure's size is less than
 * EEPROM_SIZE. It will give an error because of the negative size 
 * if sizeof(settings_t) > EEPROM_SIZE
 */
typedef char 
ERROR_no_space_in_EEPROM[(sizeof(settings_t) <= EEPROM_SIZE) ? 1 : -1];

//TODO: these are testvals
//these get stored in section ".eeprom", so they need seperate flashing
static settings_t settings_eeprom EEMEM = {
	.moisture_wanted = 500,
	.moisture_top = 0,
	.moisture_bottom = 0,
	.water_interval = 300,
	.log_interval = 60,
	.comm_address = COMM_ADDRESS
};

// the address of a field in EEPROM
#define setting_address(field) &((&settings_eeprom)->field)

static settings_t settings;

void settings_init(){
	eeprom_read_block(&settings, &settings_eeprom, sizeof(settings_t));
}

																					   /* === moisture wanted === */
uint16_t settings_get_moisture_wanted(){
	return settings.moisture_wanted;
}
void settings_set_moisture_wanted(uint16_t value){
	eeprom_update_word(setting_address(moisture_wanted), value);
	settings.moisture_wanted = value;
}

                                            /* === moisture top === */
uint16_t settings_get_moisture_top(){
	return settings.moisture_top;
}
void settings_set_moisture_top(uint16_t value){
	eeprom_update_word(setting_address(moisture_top), value);
	settings.moisture_top = value;
}

                                            /* === moisture bottom === */
uint16_t settings_get_moisture_bottom(){
	return settings.moisture_bottom;
}
void settings_set_moisture_bottom(uint16_t value){
	eeprom_update_word(setting_address(moisture_bottom), value);
	settings.moisture_bottom = value;
}

                                            /* === water interval === */
uint32_t settings_get_water_interval(){
	return settings.water_interval;
}
void settings_set_water_interval(uint32_t value){
	eeprom_update_dword(setting_address(water_interval), value);
	settings.water_interval = value;
}

                                            /* === log interval === */
uint32_t settings_get_log_interval(){
	return settings.log_interval;
}
void settings_set_log_interval(uint32_t value){
	eeprom_update_dword(setting_address(log_interval), value);
	settings.log_interval = value;
}

comm_addr_t settings_get_comm_address(){
	return settings.comm_address;
}

void settings_set_comm_address(comm_addr_t addr){
	eeprom_update_byte(setting_address(comm_address), addr);
	settings.comm_address = addr;
}
