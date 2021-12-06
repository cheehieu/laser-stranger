#include "sk6812.h"

void set_led_pixel_RGB(uint16_t pos, uint8_t r, uint8_t g, uint8_t b) {
    m_led_buffer_tx[pos].r = r;
    m_led_buffer_tx[pos].g = g;
    m_led_buffer_tx[pos].b = b;
}
