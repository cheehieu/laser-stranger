#ifndef SK6812_I2S_H_
#define SK6812_I2S_H_

#include <stdio.h>
#include <string.h>
#include "dio6305b.h"
#include "nrf_drv_i2s.h"
#include "sk6812.h"

#if defined(BOARD_EG_v0_4) || defined(BOARD_EG_v1_1) || defined(BOARD_EG_v1_2)
    #include "elliegrid_v1_x.h"
#else
#error "BOARD IS NOT DEFINED"
#endif

#define I2S_SK6812_ZERO		    0x8		//0b'1000 (0.3us; 0.9us)
#define I2S_SK6812_ONE		    0xC		//0b'1100 (0.6us; 0.6us)
#define I2S_WS2812B_ZERO	    0x8		//0b'1000 (0.4us; 0.85us)
#define I2S_WS2812B_ONE		    0xE		//0b'1110 (0.8us; 0.45us)
#define I2S_BITS_PER_DATA_BIT	    4
#define I2S_BYTES_PER_RESET	    256/8	//reset_pulse / period = 80us / 0.3125us = 256	// TRY: 264 for extra delay
#define I2S_LEDS_WORD_SIZE	    BYTES_TO_WORDS(LEDS_DATA_BYTE_SIZE * I2S_BITS_PER_DATA_BIT)   //63 words
#define I2S_RESET_WORD_SIZE	    BYTES_TO_WORDS(I2S_BYTES_PER_RESET)		//8 words
#define I2S_LEDS_FRAME_WORD_SIZE    I2S_LEDS_WORD_SIZE + I2S_RESET_WORD_SIZE	//63+8=71 words

// Global arrays to hold LED and I2S data
extern sk6812_led_t m_led_buffer_tx[NUM_LEDS];
extern uint32_t m_i2s_led_buffer_tx[I2S_LEDS_FRAME_WORD_SIZE];
extern volatile uint8_t g_i2s_data_sent_flag;
extern bool g_leds_active;

/**
 * @brief Clears the LED and I2S data buffers 
 */
uint32_t sk6812_i2s_init_mem();

/**
 * @brief Initializes the I2S interface for ~3.2MHz clock for SK6812
 */
uint32_t sk6812_i2s_init(uint16_t sdout_pin);

/**
 * @brief Stops data transfer after the first full transmission (only using single buffer)
 * @brief Handles transmitted data when receiving the TXPTRUPD and RXPTRUPD events, every RXTXD.MAXCNT number of transmitted data words
 * @brief Addresses written to the pointer registers TXD.PTR and RXD.PTR are double-buffered in hardware, and these double buffers are updated for every RXTXD.MAXCNT words
 */
void data_handler(nrf_drv_i2s_buffers_t const * p_released, uint32_t status);
 

/**
 * @brief Convert a byte of LED data to a word of I2S data 
 * @brief 1 data bit   <--> 4 I2S bits
 * @brief 1 data byte  <--> 1 I2S word
 */
uint32_t convert_byte_to_i2s_bits(uint8_t data_byte);
 
/**
 * @brief Sets I2S data from converted LED data
 */
void set_i2s_led_data(void);

/**
 * @brief Initializes I2S and starts the data transfer
 * @brief Assumes data buffers have already been set
 */
void send_i2s_led_data(uint16_t sdout_pin);

#endif // SK6812_I2S_H_
