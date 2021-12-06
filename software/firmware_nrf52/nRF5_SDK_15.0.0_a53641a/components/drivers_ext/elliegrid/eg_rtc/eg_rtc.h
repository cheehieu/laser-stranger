#ifndef EG_RTC_H_
#define EG_RTC_H_

/*
 * Driver for the nRF52x RTC with 32kHz LFCLK
 */

#include <time.h>
#include <stdint.h>
#include "app_timer.h"

#define MIN_RTC_TIME    1500000000	// Minimum valid time (indicates time not synced)

/**
 * @brief Initialize the RTC with external 32kHZ XTAL    
 */
void rtc_init(void);

/**
 * @brief Start the RTC application timer   
 */
void rtc_start(void);

/**
 * @brief Stop the RTC application timer  
 */
void rtc_stop(void);

/**
 * @brief Set the RTC current_time to a value 
 */
void rtc_set(uint32_t val);

/**
 * @brief Get the RTC current_time 
 */
uint32_t rtc_get(void);

/**
 * @brief Set the RTC match_time to a value 
 */
void rtc_set_match(uint32_t val);

/**
 * @brief Get the RTC match_time 
 */
uint32_t rtc_get_match(void);

/**
 * @brief Increment current time by 1 second   
 */
void timer_rtc_handler(void * p_context);

#endif //EG_RTC_H_
