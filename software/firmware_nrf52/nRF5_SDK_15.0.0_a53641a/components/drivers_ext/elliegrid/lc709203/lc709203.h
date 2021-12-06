#ifndef LC709203_H_
#define LC709203_H_
 
/**
 * Driver for the LC709203 Fuel Gauge
 */

#include "nrf_drv_gpiote.h"
#include "nrf_drv_twi.h"
#include "nrf_log.h"

#if defined(BOARD_EG_v0_4) || defined(BOARD_EG_v1_1) || defined(BOARD_EG_v1_2)
    #include "elliegrid_v1_x.h"
#else
#error "BOARD IS NOT DEFINED"
#endif

#define LC709203_ADDR			(0x16U >> 1)
#define LC7_REGVAL_RSOC			0xAA55
#define LC7_REGVAL_APA			0x19
#define LC7_REGVAL_BATTYPE1		0x0001
#define LC7_REGVAL_RSOC_ALARM		10      // ALARMB pin low when RSOC=10%
#define LC7_REGVAL_CELL_TEMP_N20	0x09E4
#define LC7_REGVAL_CELL_TEMP_0		0x0AAC
#define LC7_REGVAL_CELL_TEMP_25		0x0BA6
#define LC7_REGVAL_CELL_TEMP_60		0x0D04
#define MIN_BAT_CELL_VOLTAGE_MV		3000

static const nrf_drv_twi_t m_twi_lc709203 = NRF_DRV_TWI_INSTANCE(0);

/**
 * @enum The power option of lc709203. In Sleep mode only communication works     
 */
typedef enum {
    LC7_POWER_OPERATIONAL = 1,
    LC7_POWER_SLEEP = 2, 
} LC7_POWER_MODE;

/**
 * @brief Initializes the fuel gauge
 * @return value 1 means initialization is complete, 0 means it is not      
 */
ret_code_t fg_init(void);

/**
 * @brief Interrupt handler for ALARMB 
 */
void fg_INTn_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

/**
 * @brief Executes RSOC initialization
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_force_init_of_RSOC(void);

/**
 * @brief  Sets adjustment value for a battery type to improve the RSOC precision
 * see page 12 of the following document http://www.onsemi.com/pub_link/Collateral/LC709203F-D.PDF
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_set_capacity_APA(const uint16_t apa);

/**
 * @brief Reads Cell voltage in mV
 * @param result pointer that will contain the result voltage
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_read_cell_voltage(uint16_t* result);

/**
 * @brief Reads RSOC value based on a 0-100 scale 
 * @param result pointer that will contain the result voltage
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_read_RSOC(uint16_t* result);

/**
 * @brief Reads RSOC value based on a 0.0-100.0 scale (resolution of 0.1%)
 * @param result pointer that will contain the result voltage
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_read_ITE(uint16_t* result);

/**
 * @brief Sets Alarm Low RSOC value based on a 0-100 scale and reads it back as a verification 
 * If RSOC drops below this level, the fuel gauge will set low its ALARMB pin
 * @param threshold the data to write in the register
 * @return value 1 means the write operation was successful, the written and the read values match
 *         value 0 means the write operation was not successful, the written and the read values do not match
*/
uint8_t fg_set_threshold_RSOC(const uint16_t threshold);

/**
 * @brief Reads Alarm Low RSOC value based on a 0-100 scale. If RSOC drops below this level, the fuel gauge will set low its ALARMB pin
 * @param result pointer that will contain the result voltage
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_read_threshold_RSOC(uint16_t* result);

/**
 * @brief Sets Alarm Low Voltage value based on a 0-100 scale and reads it back as a verification 
 * If the measured voltage drops below this level, the fuel gauge will set low its ALARMB pin
 * @param threshold the data to write in the register
 * @return value 1 means the write operation was successful, the written and the read values match
 *         value 0 means the write operation was not successful, the written and the read values do not match
*/
uint8_t fg_set_threshold_voltage(const uint16_t threshold);

/**
 * @brief Reads Alarm Low Voltage value based on a 0-100 scale. 
 * If the measured voltage drops below this level, the fuel gauge will set low its ALARMB pin
 * @param result pointer that will contain the result voltage
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_read_threshold_voltage(uint16_t* result);

/**
 * @brief Sets the power mode
 * @param mode the power mode to be set. Can be LC7_POWER_OPERATIONAL or LC7_POWER_SLEEP
 * @return value 1 means the write operation was successful, the written and the read values match
 *         value 0 means the write operation was not successful, the written and the read values do not match
*/
uint8_t fg_set_power_mode(const LC7_POWER_MODE mode);

/**
 * @brief Reads the power mode
 * @param result containes the setted mode. Can be LC7_POWER_OPERATIONAL or LC7_POWER_SLEEP
 * @return value 1 means the write operation was successful, the written and the read values match
 *         value 0 means the write operation was not successful, the written and the read values do not match
*/
uint8_t fg_read_power_mode(uint16_t* result);

#endif /*LC709203_H_*/
