#ifndef EG_LEDS_H
#define EG_LEDS_H

/* 
 * Driver for EllieGrid compartment LEDs 
 */

#include <stdio.h>
#include "app_timer.h"
#include "dio6305b.h"
#include "eg_rtc.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "sk6812.h"
#include "sk6812_i2s.h"

#if defined(BOARD_EG_v0_4) || defined(BOARD_EG_v1_1) || defined(BOARD_EG_v1_2)
    #include "elliegrid_v1_x.h"
#else
#error "BOARD IS NOT DEFINED"
#endif

#define DEFAULT_LED_R                               255
#define DEFAULT_LED_G                               255
#define DEFAULT_LED_B                               255
#define DEFAULT_LED_PATTERN                         LED_SEL_PATTERN_ALLONOFF
#define DEFAULT_LED_SEL_PATTERN                     LED_SEL_PATTERN_123OFF
#define DEFAULT_LED_SELECT_DURATION_MS		    500
#define DEFAULT_LED_SELECT_SNAKE_DURATION_MS	    50
#define LED_SELECT_RESET_DURATION_MS		    500
#define RESET_LED_R				    255
#define RESET_LED_G				    0
#define RESET_LED_B				    0
#define BATTERY_LED_R				    0
#define BATTERY_LED_G				    100
#define BATTERY_LED_B				    0
#define DEFAULT_CHAR_DURATION_MS                    1000
#define DEFAULT_INTER_CHAR_DURATION_MS              250
#define DEFAULT_SPACE_DURATION_MS                   1000

#define LED_SEL_PATTERN_1ON1OFF			    1
#define LED_SEL_PATTERN_3ON3OFF			    2
#define LED_SEL_PATTERN_123OFF			    3
#define LED_SEL_PATTERN_ALL1ONOFF		    4
#define LED_SEL_PATTERN_ALL3ONOFF		    5
#define LED_SEL_PATTERN_ALL123			    6
#define LED_SEL_PATTERN_SNAKE			    7
#define LED_SEL_PATTERN_ALLONOFF                    8

// TODO: Implement other LED patterns leveraging app_timers implementation
#define LED_PATTERN_APPEAR			    1
#define LED_PATTERN_COLOR_WIPE			    2
#define LED_PATTERN_RAINBOW			    3
#define LED_PATTERN_RAINBOW_CYCLE		    4
#define LED_PATTERN_THEATER_CHASE		    5
#define LED_PATTERN_THEATER_CHASE_RAINBOW	    6
#define LED_PATTERN_1ON1OFF			    7
#define LED_PATTERN_3ON3OFF			    8
#define LED_PATTERN_123OFF			    9
#define LED_PATTERN_ALL1ONOFF			    10
#define LED_PATTERN_ALL3ONOFF			    11
#define LED_PATTERN_ALL123			    12
#define LED_PATTERN_SNAKE			    13
#define LED_PATTERN_RANDOM			    14
#define LED_PATTERN_SPARKLE			    15
#define LED_PATTERN_HEARTBEAT			    16

#define BAT_LEVEL_LED_1				    20	    //bottom left LED
#define BAT_LEVEL_LED_10			    11	    //bottom right LED
#define BATTERY_DISPLAY_TIME_MS			    3000    //display for 3 seconds

extern uint8_t g_led_r;
extern uint8_t g_led_g;
extern uint8_t g_led_b;
extern uint8_t g_last_rsoc;
extern uint32_t g_char_duration_ms;
extern uint32_t g_inter_char_duration_ms;
extern uint32_t g_space_duration_ms;

/**
 * @brief Initialize the LEDs, app_timers for select and battery display
 */
void eg_leds_init(void);

/**
 * @brief Stop LED timers, turn off any LEDs, turn off DC2
 */
void eg_leds_off(void);

/**
 * @brief Set to all I2S_SK6812_ZERO's
 */
void clear_leds(void);

/**
 * @brief Set neopixel data for a compartment's LEDs
 */
void set_compartment_leds(uint8_t compartment, uint8_t val, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Illuminate the LEDs, addressed by compartment
 */
void drive_compartment_leds(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Illuminate the LEDs, addressed by command LED code
 * @brief (uint32t 0x X-C7-C6-C5 C4-C3-C2-C1), where CX represents # of LEDs on in that compartment (0-3)... extended 4-7
 */
void drive_app_compartment_leds(uint32_t leds_data, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Blink all LEDs ms_delay on/off, num_blink times
 */
void blink_compartment_leds(uint8_t num_blink, uint32_t ms_delay, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Blink offset LEDs ms_delay on/off, num_blink times
 */
void blink_offset_compartment_leds(uint8_t num_blink, uint32_t ms_delay);

/**
 * @brief Illuminate and turn off LEDs sequentially in a circle
 * @brief ms_delay between neighboring LEDs
 */
void color_wipe(uint8_t r, uint8_t g, uint8_t b, uint8_t ms_delay);

/**
 * @brief Illuminate a single LED, n, in the LED chain
 */
void eg_set_single_led(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint16_t sdout_pin);

/**
 * @brief Ported Adafruit NeoPixel paint animation functions
 */
sk6812_led_t wheel(uint8_t wheel_pos);
void rainbow(uint16_t ms_delay);
void rainbow_cycle(uint16_t ms_delay);
void theater_chase(uint8_t r, uint8_t g, uint8_t b, uint16_t ms_delay);
void theater_chase_rainbow(uint16_t ms_delay);

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
void display_led_battery_level(uint8_t led_bat_lvl);
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

void eg_leds_drive_bit(uint32_t test_result, uint32_t test_progress);

/**
 * @brief Functions for handling alphabet specific
 */
bool abc_validate(char c);
void abc_display_char(char c);
void abc_display_string(char str[]);
void all_leds_blink(uint8_t r, uint8_t g, uint8_t b, uint16_t on_ms_delay, uint16_t off_ms_delay);
void stranger_things_easter_egg_0(void);
void stranger_things_easter_egg_1(void);
void regina_easter_egg(void);

#endif //EG_LEDS_H
