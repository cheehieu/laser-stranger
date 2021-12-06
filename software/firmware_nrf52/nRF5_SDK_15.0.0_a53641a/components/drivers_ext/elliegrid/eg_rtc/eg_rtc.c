#include "eg_rtc.h"

time_t current_time;
time_t match_time;
APP_TIMER_DEF(m_rtc_timer_id);

void rtc_init(void) {
    current_time = 0;
    match_time = 0;
    APP_ERROR_CHECK(app_timer_create(&m_rtc_timer_id, APP_TIMER_MODE_REPEATED, timer_rtc_handler));
}

void rtc_start(void) {
    APP_ERROR_CHECK(app_timer_start(m_rtc_timer_id, APP_TIMER_TICKS(1000), NULL));
}

void rtc_stop(void) {
    APP_ERROR_CHECK(app_timer_stop(m_rtc_timer_id));
}

void rtc_set(uint32_t val) {
    current_time = val;
}

uint32_t rtc_get(void) {
    return current_time;
}

void rtc_set_match(uint32_t val) {
    match_time = val;
}

uint32_t rtc_get_match(void) {
    return match_time;
}

void timer_rtc_handler(void * p_context) {
    current_time++;
}
