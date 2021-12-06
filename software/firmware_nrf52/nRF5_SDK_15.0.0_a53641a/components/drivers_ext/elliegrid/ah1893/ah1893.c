#include "ah1893.h"

void mag_init(void) {
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    ret_code_t err_code = nrf_drv_gpiote_in_init(MAG_OUTn_PIN, &in_config, mag_interrupt_handler);
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_gpiote_in_event_enable(MAG_OUTn_PIN, true);
}

void mag_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    
    NRF_LOG_DEBUG("mag_interrupt_handler() MAGNET SENSE: %d", get_mag_sensor_state());
    
    if (get_mag_sensor_state() == LID_STATE_OPEN) {
        // Send lid open notification
        g_notify_state |= STATUS_LID_OPEN_MASK;
        ble_egs_on_button_change(m_conn_handle, &m_egs, g_notify_state);

        stranger_things_easter_egg_1();
    } else {
        // Send lid closed notification
        g_notify_state &= ~STATUS_LID_OPEN_MASK;
        ble_egs_on_button_change(m_conn_handle, &m_egs, g_notify_state);
    }
}

bool get_mag_sensor_state(void) {
    return !nrf_drv_gpiote_in_is_set(MAG_OUTn_PIN);
}
