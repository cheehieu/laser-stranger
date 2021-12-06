#ifndef COMPARTMENT_LEDS_NEO_H
#define COMPARTMENT_LEDS_NEO_H

/* 
 * Driver for EllieGrid compartment LEDs 
 */
 
/* 
Simple sweeping lid LED config (top/bottom flipped):
 ----------------
|   C1  C2  C3   |
| C4  C5  C6  C7 |
 ----------------
Compartment#: LED#
C1: D1-D3
C2: D4-D6
C3: D7-D9
C4: D21-D19
C5: D18-D16
C6: D15-D13
C7: D12-D10
*/

#include <stdio.h>
#include "app_timer.h"
#include "dio6305b.h"
#include "eg_audio.h"
#include "nrf_delay.h"
#include "sk6812.h"

#if defined(BOARD_EG_v1_1)
    #include "elliegrid_v1_1.h"
#else
#error "Board is not defined"
#endif

#define DEFAULT_ALARM_LED_R			    0
#define DEFAULT_ALARM_LED_G			    0
#define DEFAULT_ALARM_LED_B			    100
#define DEFAULT_ALARM_LED_PATTERN		    LED_PATTERN_COLOR_WIPE
#define DEFAULT_ALARM_LED_SEL_PATTERN		    LED_SEL_PATTERN_123OFF
#define DEFAULT_LED_SELECT_DURATION_MS		    500
#define DEFAULT_LED_SELECT_SNAKE_DURATION_MS	    50
#define LED_SELECT_RESET_DURATION_MS		    500
#define RESET_LED_R				    200
#define RESET_LED_G				    0
#define RESET_LED_B				    0

#define LED_SEL_PATTERN_1ON1OFF			    1
#define LED_SEL_PATTERN_3ON3OFF			    2
#define LED_SEL_PATTERN_123OFF			    3
#define LED_SEL_PATTERN_ALL1ONOFF		    4
#define LED_SEL_PATTERN_ALL3ONOFF		    5
#define LED_SEL_PATTERN_ALL123			    6
#define LED_SEL_PATTERN_SNAKE			    7

// TODO: Implement other LED patterns using app_timers
//#define LED_PATTERN_APPEAR			    1
#define LED_PATTERN_COLOR_WIPE			    2
//#define LED_PATTERN_SNAKE			    3
//#define LED_PATTERN_ALL_BLINK			    4
//#define LED_PATTERN_RANDOMIZED			    5
//#define LED_PATTERN_COLORFUL			    6
//#define LED_PATTERN_SPARKLE			    7
//#define LED_PATTERN_HEARTBEAT			    8
//#define LED_PATTERN_COLOR_SPECTRUM		    9
//#define LED_PATTERN_SPARKLE			    10

#define BAT_LEVEL_LED_1				    20	    //bottom left LED
#define BAT_LEVEL_LED_10			    11	    //bottom right LED
#define BATTERY_DISPLAY_TIME_MS			    3000    //display for 3 seconds

extern uint8_t g_alarm_led_r;
extern uint8_t g_alarm_led_g;
extern uint8_t g_alarm_led_b;
extern uint8_t g_alarm_led_pattern_sel;
extern uint8_t g_last_rsoc;
extern uint16_t g_led_select_duration_ms;
extern uint16_t g_led_select_snake_duration_ms;
extern bool g_time_invalid;

/**
 * @brief Initialize the LEDs, app_timers for select and battery display
 */
void eg_leds_init(void);

/**
 * @brief Stop LED timers, turn off any LEDs, turn off DC2
 */
void eg_leds_off(void);

/**
 * @brief Set neopixel data for a compartment's LEDs
 */
void setCompartmentLEDs_NEO(uint8_t compartment, uint8_t val, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Illuminate the LEDs, addressed by compartment
 */
void driveCompartmentLEDs_NEO(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Illuminate the LEDs, addressed by command LED code
 * @brief (uint32t 0x X-C7-C6-C5 C4-C3-C2-C1), where CX represents # of LEDs on in that compartment (0-3)... extended 4-7
 */
void driveAppCompartmentLEDs_NEO(uint32_t leds_data, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Blink all LEDs ms_delay on/off, num_blink times
 */
void blinkCompartmentLEDs_NEO(uint8_t num_blink, uint32_t ms_delay, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Blink offset LEDs ms_delay on/off, num_blink times
 */
void blinkOffsetCompartmentLEDs_NEO(uint8_t num_blink, uint32_t ms_delay);

/**
 * @brief Illuminate and turn off LEDs sequentially in a circle
 * @brief ms_delay between neighboring LEDs
 */
void colorWipe_NEO(uint8_t r, uint8_t g, uint8_t b, uint8_t ms_delay);

/**
 * @brief Illuminate a single LED, n, in the LED chain
 */
void eg_set_single_led(uint16_t n, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Perform LED animation based on selected compartment
 */
void eg_leds_animate_select(uint8_t select_led_comp, uint8_t select_led_pattern);
void eg_leds_start_led_select_timer(uint32_t timer_ms);
void eg_leds_stop_led_select_timer(void);
void led_select_timer_handler(void * p_context);

/**
 * @brief Perform LED animation for displaying the battery level
 */
void eg_leds_start_battery_display_timer(uint32_t timer_ms);
void eg_leds_stop_battery_display_timer(void);
void battery_display_timer_handler(void * p_context);
void eg_leds_animate_battery_level(void);

/**
 * @brief Turn off the LEDs after timer elapses
 */
void eg_leds_start_led_off_timer(uint32_t timer_ms);
void eg_leds_stop_led_off_timer(void);
void led_off_timer_handler(void * p_context);

#endif //COMPARTMENT_LEDS_NEO_H
