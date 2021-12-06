#ifndef AH1893_H_
#define AH1893_H_

/**
 * Driver for the AH1893 Omnipolar Hall-effect Switch
 */

#include "ble_egs.h"
#include "nrf_drv_gpiote.h"
#include "nrf_log.h"

#if defined(BOARD_EG_v0_4) || defined(BOARD_EG_v1_1) || defined(BOARD_EG_v1_2)
    #include "elliegrid_v1_x.h"
#else
#error "BOARD IS NOT DEFINED"
#endif

#define LID_STATE_OPEN      true
#define LID_STATE_CLOSED    false

extern uint8_t		    g_notify_state;
extern ble_egs_t	    m_egs;
extern uint16_t		    m_conn_handle;

/**
 * @brief Initialize the Hall sensor input GPIO pin as a GPIOTE PORT event
 */
void mag_init(void);

/**
 * @brief Interrupt handler for Hall sensor input
 */
void mag_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

/**
 * @brief Return the state of magnet sensor
 * @return HIGH if magnet is sensed (lid is open)
 * @return LOW if magnet is not sensed (lid is closed)
 */
bool get_mag_sensor_state(void);

#endif /*AH1893_H_*/
