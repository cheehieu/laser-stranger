#include "eg_twi.h"

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(0);

void eg_twi_init(void) {
    ret_code_t err_code;
    const nrf_drv_twi_config_t twi_config = {
       .scl                = I2C_SCL_PIN,
       .sda                = I2C_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };
    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
//    APP_ERROR_CHECK(err_code);

    if (err_code == NRF_SUCCESS) {
        nrf_drv_twi_enable(&m_twi);
    }
}

void eg_twi_uninit(void) {
    nrf_drv_twi_uninit(&m_twi);
}

void eg_twi_enable(void) {
    nrf_drv_twi_enable(&m_twi);
}

void eg_twi_disable(void) {
    nrf_drv_twi_disable(&m_twi);
}
