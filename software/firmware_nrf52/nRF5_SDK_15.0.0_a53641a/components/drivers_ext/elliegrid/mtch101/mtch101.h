#ifndef MTCH101_H_
#define MTCH101_H_

/**
 * Driver for the MTCH101 Single-Channel Proximity Detector
 * Init (and handling) of CAP_OUTn_PIN is done in main.c using app_button
 */

#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrf_log.h"

#if defined(BOARD_EG_v0_4) || defined(BOARD_EG_v1_1) || defined(BOARD_EG_v1_2)
    #include "elliegrid_v1_x.h"
#else
#error "BOARD IS NOT DEFINED"
#endif

#define MTCH_POWER_LOW                          0
#define MTCH_POWER_NORMAL                       1
#define RESET_PULSE_NORMAL_POWER_MS		110		// >(4.5ms + [69ms - 105ms])
#define RESET_PULSE_LOW_POWER_MS		850		// >(4.5ms + [572ms - 845ms])

/**
 * @brief Initializes the GPIOs of the capacitive sensor and enables interrupt handling
 * @param mode sets the sensor's power mode
 */
void captouch_init(uint8_t mode);

/**
 * @brief Set the sensor's initial power mode
 * @param mode MTCH_POWER_LOW (lower power) or MTCH_POWER_NORMAL (more responsive)
 */
void set_captouch_power_mode(uint8_t mode);

/**
 * @brief Resets the sensor, it takes about 6ms
 * @brief pulse duration >(4.5ms + sleep time during non-detected state) [69ms - 105ms] [572ms - 845ms]
 * @brief reset after button release?
 * @brief reset every 30s battery level update?
 * @details The MTCH101 can be stuck in a proximity-detected state in some cases, such as sudden temperature change, or higher dielectric materials (metal, wood or glass) present near the sensor.
 */
void captouch_start_reset_pulse_timer(void);
void reset_pulse_handler(void * p_context);

#endif /*MTCH101_H_*/
