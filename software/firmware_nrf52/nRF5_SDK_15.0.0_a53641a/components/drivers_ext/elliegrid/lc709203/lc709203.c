#include "lc709203.h"

typedef enum {
    LC7_ADDR_BEFORERSOC 	= 0x04,
    LC7_ADDR_THERMISTORB	= 0x06,
    LC7_ADDR_INITIALRSOC	= 0x07,
    LC7_ADDR_CELLTEMP		= 0x08,
    LC7_ADDR_CELLVOLTAGE	= 0x09,
    LC7_ADDR_CURRENTDIR		= 0x0A,
    LC7_ADDR_APA		= 0x0B,
    LC7_ADDR_APT		= 0x0C,
    LC7_ADDR_RSOC		= 0x0D,
    LC7_ADDR_ITE		= 0x0F,
    LC7_ADDR_ICVERSION		= 0x11,
    LC7_ADDR_CHANGEPARAMS	= 0x12,
    LC7_ADDR_ALARMRSOC		= 0x13,
    LC7_ADDR_ALARMVOLTAGE	= 0x14,
    LC7_ADDR_ICPOWERMODE	= 0x15,
    LC7_ADDR_STATUSBIT		= 0x16,
    LC7_ADDR_NUMOFPARAM		= 0x1A,
} LC7_REGISTER_ADDRESS;

/**
 * @brief LC709203 uses CRC8 checksum in its communication
 * @brief This lookup table is created for the CRC8 calculations
 */
static const uint8_t lc7_crc_table[] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31,
    0x24, 0x23, 0x2a, 0x2d, 0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
    0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 0xe0, 0xe7, 0xee, 0xe9,
    0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1,
    0xb4, 0xb3, 0xba, 0xbd, 0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
    0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 0xb7, 0xb0, 0xb9, 0xbe,
    0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16,
    0x03, 0x04, 0x0d, 0x0a, 0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
    0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 0x89, 0x8e, 0x87, 0x80,
    0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8,
    0xdd, 0xda, 0xd3, 0xd4, 0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
    0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 0x19, 0x1e, 0x17, 0x10,
    0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f,
    0x6a, 0x6d, 0x64, 0x63, 0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
    0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 0xae, 0xa9, 0xa0, 0xa7,
    0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef,
    0xfa, 0xfd, 0xf4, 0xf3
};

/**
 * @brief LC709203 uses CRC8 checksum in its communication. This function calculates the related checksum
 * @param p pointer of the array that contains the message to send. I should contain the whole message starting with the device address
 * @param len the lenght of the message contained by *p
 * @return the generated crc8 value
 */
uint8_t fg_crc8(uint8_t *p, uint8_t len) {
    uint16_t i;
    uint16_t crc = 0x0;

    while (len--) {
	i = (crc ^ *p++) & 0xFF;
	crc = (lc7_crc_table[i] ^ (crc << 8)) & 0xFF;
    }

    return crc & 0xFF;
}

/**
 * @brief Reads one word from the address "regaddress"
 * only works if TWI line is initialized in blocking mode
 * @param array pointer that contains the message, must be 6 bytes long
 * @param address the address of the register to read 
 */
void fg_read_word(uint8_t* array, uint8_t regaddress) {
    array [0] = LC709203_ADDR<<1;  array [1] = regaddress; array [2] = (LC709203_ADDR<<1)+1;
    nrf_drv_twi_tx(&m_twi_lc709203, LC709203_ADDR, array+1, 1, true);
    nrf_drv_twi_rx(&m_twi_lc709203, LC709203_ADDR, array+3, 3);
}

/**
 * @brief Checks if the CRC value matches or not
 * @param array pointer that contains the message
 * @param length the length of the array
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_is_CRC8_ok(uint8_t* array, uint8_t length) {
    if(array[length-1] == fg_crc8(array, length-1)) {
	 return 1;
    } else {
	 return 0;
    }
}

/**
 * @brief Reads one register
 * @param result pointer that will contain the result voltage
 * @param reg register address to read
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_read_reg(uint16_t* result, LC7_REGISTER_ADDRESS reg) {
    uint8_t regfulmsg[6];
    fg_read_word(regfulmsg, reg);
    *result = ((regfulmsg[4]<<8)+regfulmsg[3]);
    return fg_is_CRC8_ok(regfulmsg, 6);
}

/**
 * @brief Writes a register given as reg and reads its value back as a verification 
 * @param data the data to write in the register
 * @param reg register address to write
 * @return value 1 means the write operation was successful, the written and the read values match
 *         value 0 means the write operation was not successful, the written and the read values do not match
*/
uint8_t fg_write_reg(uint16_t data, LC7_REGISTER_ADDRESS reg) {
    uint8_t array[5];
    uint16_t result;
    array [0] = LC709203_ADDR<<1;  array [1] = reg; array [2] = data&0xFF; array [3] = data>>8;
    array[4]=fg_crc8(array, 4);
    nrf_drv_twi_tx(&m_twi_lc709203, LC709203_ADDR, array+1, 4, false);
    if(fg_read_reg(&result, reg) && (result == data)) {
	return 1;
    } else {
	return 0;
    }
}

/**
 * @brief Initializes the fuel gauge
 * @return value 1 means initialization is complete, 0 means it is not      
 */
//~4-ohms of parasitic resistance on VDD line? Need to compensate for RSOC error?

ret_code_t fg_init(void) {
    // Set to Operational mode [initial power mode is operational mode]
//    fg_write_reg(0x0001, LC7_ADDR_ICPOWERMODE);
	
    // Set APA to 1000mAh
    fg_write_reg(LC7_REGVAL_APA, LC7_ADDR_APA);
    
    // Set battery profile to 3.7V/4.2V
    fg_write_reg(LC7_REGVAL_BATTYPE1, LC7_ADDR_CHANGEPARAMS);
    
    // Initial RSOC
    fg_force_init_of_RSOC();
    
    // Set via I2C mode
    fg_write_reg(0x0000, LC7_ADDR_STATUSBIT);
    
    // Set cell temperature [default at 25C]
    fg_write_reg(LC7_REGVAL_CELL_TEMP_25, LC7_ADDR_CELLTEMP);
    
    // Init ALARMB as GPIOTE PORT event [interrupt pin goes low, but only when reading RSOC?]
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    nrf_drv_gpiote_in_init(FG_INTn_PIN, &in_config, fg_INTn_handler);
    nrf_drv_gpiote_in_event_enable(FG_INTn_PIN, true);
    
    // Battery voltage shoots up when USB is connected --> RSOC is unreliable while charging
    // low=0-15%, med=16-79%, high=80-100%
    // low-to-med=15minutes; med-to-high=30minutes; low-to-high=1hr30minutes		
    
    return NRF_SUCCESS;
}

/**
 * @brief Interrupt handler for ALARMB, interrupt on charging from LP_MODE_THRESHOLD (e.g. 9% to 10%, and on discharging from 10% to 9%)
 */
void fg_INTn_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    uint16_t rsoc = 0;
    fg_read_RSOC(&rsoc);
    NRF_LOG_DEBUG("fg_INTn_handler() RSOC: %d%%", (uint8_t) rsoc);
}

/**
 * @brief Executes RSOC initialization
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_force_init_of_RSOC(void) {
    return fg_write_reg(LC7_REGVAL_RSOC, LC7_ADDR_INITIALRSOC);
}

/**
 * @brief  Sets adjustment value for a battery type to improve the RSOC precision
 * see page 12 of the following document http://www.onsemi.com/pub_link/Collateral/LC709203F-D.PDF
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_set_capacity_APA(const uint16_t apa) {
    return fg_write_reg(apa, LC7_ADDR_APA);
}

/**
 * @brief Reads Cell voltage in mV
 * @param result pointer that will contain the result voltage
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_read_cell_voltage(uint16_t* result) {
    return fg_read_reg(result, LC7_ADDR_CELLVOLTAGE);
}

/**
 * @brief Reads RSOC value based on a 0-100 scale 
 * @param result pointer that will contain the result voltage
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_read_RSOC(uint16_t* result) {
    return fg_read_reg(result, LC7_ADDR_RSOC);
}

/**
 * @brief Reads RSOC value based on a 0.0-100.0 scale (resolution of 0.1%)
 * @param result pointer that will contain the result voltage
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_read_ITE(uint16_t* result) {
    return fg_read_reg(result, LC7_ADDR_ITE);
}

/**
 * @brief Sets Alarm Low RSOC value based on a 0-100 scale and reads it back as a verification 
 * If RSOC drops below this level, the fuel gauge will set low its ALARMB pin
 * @param threshold the data to write in the register
 * @return value 1 means the write operation was successful, the written and the read values match
 *         value 0 means the write operation was not successful, the written and the read values do not match
*/
uint8_t fg_set_threshold_RSOC(const uint16_t threshold) {
    return fg_write_reg(threshold, LC7_ADDR_ALARMRSOC);
}

/**
 * @brief Reads Alarm Low RSOC value based on a 0-100 scale. If RSOC drops below this level, the fuel gauge will set low its ALARMB pin
 * @param result pointer that will contain the result voltage
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_read_threshold_RSOC(uint16_t* result) {
    return fg_read_reg(result, LC7_ADDR_ALARMRSOC);
}

/**
 * @brief Sets Alarm Low Voltage value based on a 0-100 scale and reads it back as a verification 
 * If the measured voltage drops below this level, the fuel gauge will set low its ALARMB pin
 * @param threshold the data to write in the register
 * @return value 1 means the write operation was successful, the written and the read values match
 *         value 0 means the write operation was not successful, the written and the read values do not match
*/
uint8_t fg_set_threshold_voltage(const uint16_t threshold) {
    return fg_write_reg(threshold, LC7_ADDR_ALARMVOLTAGE);
}

/**
 * @brief Reads Alarm Low Voltage value based on a 0-100 scale. 
 * If the measured voltage drops below this level, the fuel gauge will set low its ALARMB pin
 * @param result pointer that will contain the result voltage
 * @return value 1 means the result is valid, 0 means it is invalid      
 */
uint8_t fg_read_threshold_voltage(uint16_t* result) {
    return fg_read_reg(result, LC7_ADDR_ALARMVOLTAGE);
}

/**
 * @brief Sets the power mode
 * @param mode the power mode to be set. Can be LC7_POWER_OPERATIONAL or LC7_POWER_SLEEP
 * @return value 1 means the write operation was successful, the written and the read values match
 *         value 0 means the write operation was not successful, the written and the read values do not match
*/
uint8_t fg_set_power_mode(const LC7_POWER_MODE mode) {
    return fg_write_reg(mode, LC7_ADDR_ICPOWERMODE);
}

/**
 * @brief Reads the power mode
 * @param result containes the setted mode. Can be LC7_POWER_OPERATIONAL or LC7_POWER_SLEEP
 * @return value 1 means the write operation was successful, the written and the read values match
 *         value 0 means the write operation was not successful, the written and the read values do not match
*/
uint8_t fg_read_power_mode(uint16_t* result) {
    return fg_read_reg(result, LC7_ADDR_ICPOWERMODE);
}
