#include "dio6305b.h"

void dc2_init(void) {
    nrf_gpio_cfg_output(DC2_EN_PIN);
}

void dc2_enable(void) {
    if (dc2_state() == DC2_OFF) {
	nrf_gpio_pin_set(DC2_EN_PIN);
	nrf_delay_us(500); //wait ~400us for Vout supply to stabilize
    }
}

void dc2_disable(void) {
    if (dc2_state() == DC2_ON) {
	nrf_gpio_pin_clear(DC2_EN_PIN);
	nrf_delay_us(500); //wait ~400us for Vout supply to stabilize
    }
}

bool dc2_state(void) {
    return nrf_gpio_pin_out_read(DC2_EN_PIN);
}
