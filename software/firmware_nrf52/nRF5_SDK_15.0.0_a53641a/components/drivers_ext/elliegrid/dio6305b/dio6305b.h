#ifndef DIO6305B_H_
#define DIO6305B_H_

/*
 * Driver for the DIO6305B 1.2MHz, 1.1A Synchronous Step-Up Converter
 * This DC-DC Vout powers the 5V peripherals (LEDs, audio)
*/

#include "nrf_gpio.h"
#include "nrf_delay.h"

#if defined(BOARD_EG_v0_4) || defined(BOARD_EG_v1_1) || defined(BOARD_EG_v1_2)
    #include "elliegrid_v1_x.h"
#else
#error "BOARD IS NOT DEFINED"
#endif

#define DC2_ON	    1
#define DC2_OFF	    0

/**
 * @brief Initialize DC2_EN_PIN as a GPIO output
 */
void dc2_init(void);

/**
 * @brief Set DC2_EN_PIN to put DCDC in active mode (~30uA quiescent)
 */
void dc2_enable(void);

/**
 * @brief Clear DC2_EN_PIN to put DCDC in shutdown mode (~1uA, load disconnect)
 * @brief Takes ~10s to complete... consider using FET in next HW version
 */
void dc2_disable(void);

/**
 * @brief Reads the enable pin to return state of DC2 output
 * @return HIGH if DC2 enabled (ON), LOW if DC2 disabled (OFF)
 */
bool dc2_state(void);

#endif // DIO6305B_H_
