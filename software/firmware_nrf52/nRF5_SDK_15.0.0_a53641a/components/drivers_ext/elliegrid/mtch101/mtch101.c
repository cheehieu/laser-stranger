#include "mtch101.h"

APP_TIMER_DEF(m_reset_pulse_timer_id);

void captouch_init(uint8_t mode) {
    // Init MTPM GPIO output and set power mode
    nrf_gpio_cfg_output(CAP_MTPM_PIN);
    set_captouch_power_mode(mode);
    
    // Init timer for reset pulse
    ret_code_t err_code = app_timer_create(&m_reset_pulse_timer_id, APP_TIMER_MODE_SINGLE_SHOT, reset_pulse_handler);
    APP_ERROR_CHECK(err_code);
}

void set_captouch_power_mode(uint8_t mode) {
    if (mode == MTCH_POWER_LOW) {
	nrf_gpio_pin_clear(CAP_MTPM_PIN);
    } else {
	nrf_gpio_pin_set(CAP_MTPM_PIN);
    }
}

void captouch_start_reset_pulse_timer(void) {
    // Adjust pulse duration depending on power mode
    uint32_t time_ms;
    if (nrf_gpio_pin_out_read(CAP_MTPM_PIN) == MTCH_POWER_LOW) {
	time_ms = RESET_PULSE_LOW_POWER_MS;
    } else {
	time_ms = RESET_PULSE_NORMAL_POWER_MS;
    }
    
    // Toggle CAP_MTPM_PIN to begin reset pulse
    nrf_gpio_pin_toggle(CAP_MTPM_PIN);
    
    // Start pulse timer
    ret_code_t err_code = app_timer_start(m_reset_pulse_timer_id, APP_TIMER_TICKS(time_ms), NULL);
    APP_ERROR_CHECK(err_code);
}

void reset_pulse_handler(void * p_context) {
    // Toggle CAP_MTPM_PIN to end reset pulse
    nrf_gpio_pin_toggle(CAP_MTPM_PIN);
}
