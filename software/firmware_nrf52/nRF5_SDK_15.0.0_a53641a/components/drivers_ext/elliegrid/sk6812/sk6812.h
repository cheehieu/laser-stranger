#ifndef SK6812_H_
#define SK6812_H_

#include <stdint.h>

#if defined(BOARD_EG_v0_4) || defined(BOARD_EG_v1_1) || defined(BOARD_EG_v1_2)
    #include "elliegrid_v1_x.h"
#else
#error "BOARD IS NOT DEFINED"
#endif

#define DATA_BYTES_PER_LED	3   // 24-bit GRB data structure
#define LEDS_DATA_BYTE_SIZE	NUM_LEDS * DATA_BYTES_PER_LED

typedef struct {
    uint8_t g;
    uint8_t r;
    uint8_t b;
} sk6812_led_t;

extern sk6812_led_t m_led_buffer_tx[NUM_LEDS];

/**
 * @brief Set an RGB LED color data in the global sk6812_led_t array
 */
void set_led_pixel_RGB(uint16_t pos, uint8_t r, uint8_t g, uint8_t b);

#endif // SK6812_H_
