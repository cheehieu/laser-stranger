#ifndef BQ2409X_H_
#define BQ2409X_H_

/**
 * Driver for the BQ2409X 1A, Single-Input, Single Cell LiPo Battery Charger
 */

#include "ble_egs.h"
#include "eg_leds.h"
#include "eg_rtc.h"
#include "nrf_drv_gpiote.h"

#if defined(BOARD_EG_v0_4) || defined(BOARD_EG_v1_1) || defined(BOARD_EG_v1_2)
    #include "elliegrid_v1_x.h"
#else
#error "BOARD IS NOT DEFINED"
#endif

#define DEFAULT_PLUG_LED_DURATION_MS	2000

extern time_t		g_time_of_plug;
extern time_t		g_time_of_unplug;
extern time_t		g_time_of_plug_conn;
extern uint8_t		g_last_rsoc;
extern uint8_t		g_notify_state;
extern ble_egs_t	m_egs;
extern uint16_t         m_conn_handle;

/**
 * @brief Initialize the the battery charger interface
 */
void bc_init(void);

/**
 * @brief Reads the /PG battery charger state
 * @return HIGH if charger input voltage is valid (above undervoltage lock-out (UVLO) and the OUT voltage)
 * @return HIGH if charging, LOW if not charging or charge complete
 */
bool get_bc_charging_state(void);

/**
 * @brief Interrupt handler for /PG input
 * @brief Illuminate LED for DEFAULT_PLUG_LED_DURATION_MS on plug, calculate plug time
 */
void PGn_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

/**
 * @brief Start oneshot app timer for illuminating LED on plug
 */
void start_plug_timer(void);

/**
 * @brief App timer handler to turn off LED after oneshot timer elapses
 */
void plug_timer_handler(void * p_context);

/**
 * @brief Interrupt handler for /CHG input
 */
void CHGn_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

#endif /*BQ2409X_H_*/
