#ifndef EG_TWI_H_
#define EG_TWI_H_

#include "nrf_drv_twi.h"

#if defined(BOARD_EG_v0_4) || defined(BOARD_EG_v1_1) || defined(BOARD_EG_v1_2)
    #include "elliegrid_v1_x.h"
#else
#error "BOARD IS NOT DEFINED"
#endif

/**
 * @brief Initializes TWI and enables the interface
 * @details TWI0, in blocking mode, 400 kHz
 */
void eg_twi_init(void);

/**
 * @brief Uninitializes TWI
 */
void eg_twi_uninit(void);

/**
 * @brief Enables the TWI instance
 */
void eg_twi_enable(void);

/**
 * @brief Disables the TWI instance
 */
void eg_twi_disable(void);

#endif // EG_TWI_H_
