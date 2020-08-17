#ifndef ACTIVEPLANT_SETTINGS_H
#define ACTIVEPLANT_SETTINGS_H
#include <stdint.h>
#include "config.h"
#include "comm.h"

/*
 * WARNING: THESE FUNCTIONS ARE NOT INTERRUPT SAFE !!!
 *
 * Functions for getting and setting the settings of the unit,
 * including things like the wanted moisture level, watering interval
 * etc
 *
 * The settings are stored in the eeprom and cached in ram memory.
 * Writing values to eeprom and the first read of the settings are
 * blocking operations.
 *
 * The functions utilise avr-libc's implementation for reading from
 * and writing to the eeprom.
 */

// EEPROM_SIZE needs to be defined to ensure that all the values for
// the settings will fit into the eeprom memory
#ifndef  EEPROM_SIZE
#error "EEPROM_SIZE needs to be defined for settings to work"
#endif

/* read the settings from EEPROM and store them in ram for faster
 * reading. Needs to be called at the start of the program */
void settings_init();

                                                /* ===================== */
                                                /* Moisture level wanted */
                                                /* ===================== */
/* The normalized moisture level, that the unit is set to
 * mainntain in the plant */
uint16_t settings_get_moisture_wanted();
void settings_set_moisture_wanted(uint16_t moisture_level);

                                                   /* =================  */
                                                   /* Moisture top value */
                                                   /* =================  */
/* Returns the top value of the sensor used (raw value)
 * Top value is defined to be the value the sensor has
 * when it is fully submerged in water */
uint16_t settings_get_moisture_top();
void settings_set_moisture_top(uint16_t top_value);

                                                /* ===================== */
                                                /* Moisture bootom value */
                                                /* ===================== */
/* Returns the bottom value of the sensor used (raw value)
 * Bottom value is defined to be the value the sensor has
 * when it is in dry air */
uint16_t settings_get_moisture_bottom();
void settings_set_moisture_bottom(uint16_t bottom_value);

                                                    /* ================  */
                                                    /* Watering interval */
                                                    /* ================  */
/* The minimum time period in seconds that must pass between
 * two watering attempts. This means that the unit will wait
 * 'interval_seconds' before opening the pump again, even if
 * the soil moisture level indicates that the plant needs water.
 * This is done to let water values stabilize. */
uint32_t settings_get_water_interval();
void settings_set_water_interval(uint32_t seconds);

                                                     /* ================ */
                                                     /* Logging interval */
                                                     /* ================ */
/* How often the unit should log its status. */
uint32_t settings_get_log_interval();
void settings_set_log_interval(uint32_t seconds);

                                                  /* ==================== */
                                                  /* Unit Protocl Address */
                                                  /* ==================== */
comm_addr_t settings_get_comm_address();
void settings_set_comm_address(comm_addr_t addr);

#endif // ACTIVEPLANT_SETTINGS_H
