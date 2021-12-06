#include "sk6812_i2s.h"

uint32_t sk6812_i2s_init_mem() {
// TODO: dynamically allocate memory when needed instead of using global arrays
//    // Init memory for LED data
//    if (p_led_buffer_tx) free(p_led_buffer_tx);
//    p_led_buffer_tx = (sk6812_led_t *)malloc(LEDS_DATA_BYTE_SIZE);
//    if (p_led_buffer_tx) memset(p_led_buffer_tx, 0, LEDS_DATA_BYTE_SIZE);
//    else return NRF_ERROR_NO_MEM;
//    
//    // Init memory for I2S data
//    if (p_i2s_led_buffer_tx) free(p_i2s_led_buffer_tx);
//    p_i2s_led_buffer_tx = (uint32_t *)malloc(I2S_LEDS_WORD_SIZE * 4);
//    if (p_i2s_led_buffer_tx) memset(p_i2s_led_buffer_tx, 0, I2S_LEDS_WORD_SIZE * 4);
//    else return NRF_ERROR_NO_MEM;

    // Reset data buffers
    memset(m_led_buffer_tx, 0, sizeof(m_led_buffer_tx));
    memset(m_i2s_led_buffer_tx, 0, sizeof(m_i2s_led_buffer_tx));
    
    return NRF_SUCCESS;
}

uint32_t sk6812_i2s_init(uint16_t sdout_pin) {
    nrf_drv_i2s_config_t config;     //= NRF_DRV_I2S_DEFAULT_CONFIG;
    config.sck_pin	= LED_SCK_PIN;	// Don't set NRF_DRV_I2S_PIN_NOT_USED for I2S_CONFIG_SCK_PIN. (The program will stack.) 
    config.lrck_pin	= LED_LRCK_PIN; // I2S_CONFIG_LRCK_PIN; // NRF_DRV_I2S_PIN_NOT_USED;
    config.mck_pin	= NRF_DRV_I2S_PIN_NOT_USED;
    config.sdout_pin	= sdout_pin;	//LED_DIN_PIN;	// I2S_CONFIG_SDOUT_PIN
    config.sdin_pin	= NRF_DRV_I2S_PIN_NOT_USED;
    config.irq_priority = I2S_CONFIG_IRQ_PRIORITY;
    config.mode         = NRF_I2S_MODE_MASTER;
    config.format       = NRF_I2S_FORMAT_I2S;
    config.alignment    = NRF_I2S_ALIGN_LEFT;
    config.sample_width = NRF_I2S_SWIDTH_8BIT;
    config.channels     = NRF_I2S_CHANNELS_STEREO;
    config.mck_setup    = NRF_I2S_MCK_32MDIV5;
    config.ratio        = NRF_I2S_RATIO_32X;
    uint32_t err_code = nrf_drv_i2s_init(&config, data_handler);
//    APP_ERROR_CHECK(err_code);
    
    return err_code;
}

void data_handler(nrf_drv_i2s_buffers_t const * p_released, uint32_t status) {
//    NRF_LOG_DEBUG("status: %d; p_tx_buffer: 0x%08X; p_rx_buffer: 0x%08X", status, p_released->p_tx_buffer, p_released->p_rx_buffer);

    // 'nrf_drv_i2s_next_buffers_set' is called directly from the handler
    // each time next buffers are requested, so data corruption is not expected.
//    ASSERT(p_released);

    // When the handler is called after the transfer has been stopped
    // (no next buffers are needed, only the used buffers are to be
    // released), there is nothing to do.
    if (!(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED)) {
        return;
    }

    // Stop data transfer after the first full transmission (only using single buffer)
    if (p_released->p_tx_buffer != NULL) {
//	nrf_drv_i2s_stop();
	nrf_drv_i2s_uninit();
	
	// Set flag
	g_i2s_data_sent_flag = 1;
    }
}

uint32_t convert_byte_to_i2s_bits(uint8_t data_byte) {
    uint32_t data_bits = 0;
    
    // Set data_bits based on MSB, then left-shift data_byte
    for (int ii=0; ii < 8; ii++) {
	data_bits |= ((data_byte & 0x80) ? I2S_SK6812_ONE : I2S_SK6812_ZERO) << ((8-1-ii) * 4);
//	data_bits |= ((data_byte & 0x80) ? I2S_WS2812B_ONE : I2S_WS2812B_ZERO) << ((8-1-ii) * 4);
	data_byte = data_byte << 1;
    }
    return data_bits;
}

void set_i2s_led_data() {
    uint16_t jj = 0;
    for (uint16_t ii=0; ii < NUM_LEDS; ii++) {
	m_i2s_led_buffer_tx[jj] = convert_byte_to_i2s_bits(m_led_buffer_tx[ii].g);
	m_i2s_led_buffer_tx[jj+1] = convert_byte_to_i2s_bits(m_led_buffer_tx[ii].r);
	m_i2s_led_buffer_tx[jj+2] = convert_byte_to_i2s_bits(m_led_buffer_tx[ii].b);
	jj += 3;
    }
}

void send_i2s_led_data(uint16_t sdout_pin) {
    // Reset flag
    g_i2s_data_sent_flag = 0;
    
    // Enable 5V supply for LEDs
    if (dc2_state() == DC2_OFF) {
	dc2_enable();
    }

    // Configure the I2S module and map IO pins
    uint32_t err_code = sk6812_i2s_init(sdout_pin);
    
    // Prevent starting a new data transfer if I2S already initialized
    if (err_code == NRF_SUCCESS) {
	// Configure TX data buffer
	nrf_drv_i2s_buffers_t const initial_buffers = {
	    .p_tx_buffer = m_i2s_led_buffer_tx,
	    .p_rx_buffer = NULL
	};

        // Add delay to prevent LED display error (brighter reset LED as DC2 ramps up?)
	nrf_delay_us(50);

	// Enable the I2S module and start data streaming
	err_code = nrf_drv_i2s_start(&initial_buffers, I2S_LEDS_FRAME_WORD_SIZE+1, 0);
	APP_ERROR_CHECK(err_code);
    } else {
	// Reset to all I2S_SK6812_ZERO's
	memset(m_i2s_led_buffer_tx, 0x88, I2S_LEDS_WORD_SIZE * 4);
    }
}
