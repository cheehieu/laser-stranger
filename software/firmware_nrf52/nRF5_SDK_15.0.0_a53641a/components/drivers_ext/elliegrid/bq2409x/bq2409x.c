#include "bq2409x.h"

APP_TIMER_DEF(m_plug_timer_id);

void bc_init(void) {
    // Init BC_PGn_PIN as GPIOTE PORT event
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    ret_code_t err_code = nrf_drv_gpiote_in_init(BC_PGn_PIN, &in_config, PGn_interrupt_handler);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(BC_PGn_PIN, true);
	
    // Init BC_CHGn_PIN	as GPIOTE PORT event
    err_code = nrf_drv_gpiote_in_init(BC_CHGn_PIN, &in_config, CHGn_interrupt_handler);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(BC_CHGn_PIN, true);

    // Init plug LED timer
    err_code = app_timer_create(&m_plug_timer_id, APP_TIMER_MODE_SINGLE_SHOT, plug_timer_handler);
    APP_ERROR_CHECK(err_code);
}

bool get_bc_charging_state(void) {
    return !nrf_drv_gpiote_in_is_set(BC_PGn_PIN);
}

void PGn_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {    

    // On a USB plug event
    if (get_bc_charging_state() == true) {
	
	// Battery charge tracking
	g_time_of_plug = rtc_get();
	NRF_LOG_DEBUG("Plug detected at %d%% [%d]", g_last_rsoc, g_time_of_plug);

	// Send charging notification
	g_notify_state |= STATUS_CHARGING_MASK;
	ble_egs_on_button_change(m_conn_handle, &m_egs, g_notify_state);
    } else {
	// Battery charge tracking
	g_time_of_unplug = rtc_get();
	g_time_of_plug_conn = g_time_of_unplug - g_time_of_plug;
	NRF_LOG_DEBUG("Unplug detected at %d%% [%d]", g_last_rsoc, g_time_of_unplug);
	NRF_LOG_DEBUG("Plug connection time: %d seconds", g_time_of_plug_conn);
	
	// Send not-charging notification
	g_notify_state &= ~STATUS_CHARGING_MASK;
	ble_egs_on_button_change(m_conn_handle, &m_egs, g_notify_state);
    }
}

void start_plug_timer(void) {
    ret_code_t err_code = app_timer_start(m_plug_timer_id, APP_TIMER_TICKS(DEFAULT_PLUG_LED_DURATION_MS), NULL);
    APP_ERROR_CHECK(err_code);
}

void plug_timer_handler(void * p_context) {
    eg_leds_off();
}

void CHGn_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    NRF_LOG_DEBUG("BC CHGn Interrupt! pin: 0x%x, polarity: 0x%x, val: %d", pin, action, nrf_drv_gpiote_in_is_set(BC_CHGn_PIN));
}
