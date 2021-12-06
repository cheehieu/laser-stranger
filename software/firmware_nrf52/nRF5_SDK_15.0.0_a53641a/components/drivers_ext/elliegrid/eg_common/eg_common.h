#ifndef EG_COMMON_H_
#define EG_COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include "eg_flash.h"

#define MAX_UINT32		        0xFFFFFFFF
#define MIN_UINT32		        0x00000000
#define MAX_SINT32		        0x7FFFFFFF
#define MIN_SINT32		        0xFFFFFFFF
#define MAX_UINT8		        0xFF
#define MIN_UINT8		        0x00
#define MAX_SINT8		        0x7F
#define MIN_SINT8		        0xFF

#define MIN_PARAM_DN_CHAR		RX_STR_LEN_OFFSET
#define MAX_PARAM_DN_CHAR		MAX_UINT8
#define MIN_PARAM_DN_LEN		0
#define MAX_PARAM_DN_LEN		MAX_DEVICE_NAME_LENGTH

//static inline bool eg_validate_byte(uint8_t val, uint8_t min, uint8_t max) {
//    if (val>=min && val<=max) {
//	return true;
//    } else {
//	return false;
//    }
//}
//
//static inline bool eg_validate_word(uint32_t val, uint32_t min, uint32_t max) {
//    if (val>=min && val<=max) {
//	return true;
//    } else {
//	return false;
//    }
//}
	
bool eg_validate_byte(uint8_t val, uint8_t min, uint8_t max);
bool eg_validate_word(uint32_t val, uint32_t min, uint32_t max);
	

///**@brief Macro for verifying statement to be true. It will cause the exterior function to return
// *        err_code if the statement is not true.
// *
// * @param[in]   statement   Statement to test.
// * @param[in]   err_code    Error value to return if test was invalid.
// *
// * @retval      nothing, but will cause the exterior function to return @p err_code if @p statement
// *              is false.
// */
//#define VERIFY_TRUE(statement, err_code)    \
//do                                          \
//{                                           \
//    if (!(statement))                       \
//    {                                       \
//        return err_code;                    \
//    }                                       \
//} while (0)

///**@brief Check if valid device name char (UTF-8 string value)
// * @return If invalid, return NRF_ERROR_INVALID_PARAM
// */

//#define VERIFY_PARAM_DN_CHAR(param)									\
//if ( ((param) < MIN_PARAM_DN_CHAR) || ((param) > MAX_PARAM_DN_CHAR) ) {				\
//    NRF_LOG_DEBUG("ERROR. Tried to set invalid device name character: 0x%X\r\n", (param));		\
//    break;												\
//}
//
///**@brief Check if valid device name length (UTF-8 string value)
// * @return If invalid, return NRF_ERROR_INVALID_PARAM
// */
//
//#define VERIFY_PARAM_DN_LEN(param)									\
//if (((param) >= MIN_PARAM_DN_LEN) && ((param) <= MAX_PARAM_DN_LEN)) {					\
//    return true;											\
//} else {												\
//    NRF_LOG_DEBUG("ERROR. Tried to set invalid device name length: %d\r\n", (param));			\
//    return false;											\
//}


/**@brief Check if valid year (years since 1900)
 * @return If invalid, return NRF_ERROR_INVALID_PARAM
 */
#define MIN_PARAM_YEAR			MIN_UINT8		//>2017?
#define MAX_PARAM_YEAR			MAX_UINT8
#define VERIFY_PARAM_YEAR(param)									\
if (((param) < MIN_PARAM_YEAR) || ((param) > MAX_PARAM_YEAR)) {						\
    NRF_LOG_DEBUG("ERROR. Tried to set invalid year: %d\r\n", (param));					\
    return NRF_ERROR_INVALID_PARAM;									\
}

/**@brief Check if valid month (months since January, 0 to 11)
 * @return If invalid, return NRF_ERROR_INVALID_PARAM
 */
#define MIN_PARAM_MONTH			0
#define MAX_PARAM_MONTH			11
#define VERIFY_PARAM_MONTH(param)									\
if ((param) > MAX_PARAM_MONTH) {									\
    NRF_LOG_DEBUG("ERROR. Tried to set invalid month: %d\r\n", (param));				\
    return NRF_ERROR_INVALID_PARAM;									\
}

/**@brief Check if valid date (day of the month, 1 to 31)
 * @return If invalid, return NRF_ERROR_INVALID_PARAM
 */
#define MIN_PARAM_DATE			1
#define MAX_PARAM_DATE			31
#define VERIFY_PARAM_DATE(param)									\
if (((param) < MIN_PARAM_DATE) || ((param) > MAX_PARAM_DATE)) {						\
    NRF_LOG_DEBUG("ERROR. Tried to set invalid date: %d\r\n", (param));					\
    return NRF_ERROR_INVALID_PARAM;									\
}

/**@brief Check if valid hour (hours since midnight, 0 to 23)
 * @return If invalid, return NRF_ERROR_INVALID_PARAM
 */
#define MIN_PARAM_HOUR			0
#define MAX_PARAM_HOUR			23
#define VERIFY_PARAM_HOUR(param)									\
if ((param) > MAX_PARAM_HOUR) {										\
    NRF_LOG_DEBUG("ERROR. Tried to set invalid hour: %d\r\n", (param));					\
    return NRF_ERROR_INVALID_PARAM;									\
}

/**@brief Check if valid minute (minutes after the hour, 0 to 59)
 * @return If invalid, return NRF_ERROR_INVALID_PARAM
 */
#define MIN_PARAM_MINUTE		0
#define MAX_PARAM_MINUTE		59
#define VERIFY_PARAM_MINUTE(param)									\
if ((param) > MAX_PARAM_MINUTE) {									\
    NRF_LOG_DEBUG("ERROR. Tried to set invalid minute: %d\r\n", (param));				\
    return NRF_ERROR_INVALID_PARAM;									\
}

/**@brief Check if valid epoch (int32_t unix secs past 1-Jan-70)
 * @return If invalid, return NRF_ERROR_INVALID_PARAM
 */
#define MIN_PARAM_EPOCH			MIN_SINT32
#define MAX_PARAM_EPOCH			MAX_SINT32
#define VERIFY_PARAM_EPOCH(param)									\
if (((param) < MIN_PARAM_MINUTE) || ((param) > MAX_PARAM_MINUTE)) {					\
    NRF_LOG_DEBUG("ERROR. Tried to set invalid epoch: %d\r\n", (param));				\
    return NRF_ERROR_INVALID_PARAM;									\
}

/**@brief Check if valid battery level (0% - 100%)
 * @return If invalid, return NRF_ERROR_INVALID_PARAM
 */
#define MIN_PARAM_BAT_LEVEL		0
#define MAX_PARAM_BAT_LEVEL		100
//#define VERIFY_PARAM_BAT_LEVEL(param)									\
//if ((param) > MAX_PARAM_BAT_LEVEL) {									\
//    NRF_LOG_DEBUG("ERROR. Tried to set invalid battery level: %d%%\r\n", (param));			\
//    return NRF_ERROR_INVALID_PARAM;									\
//}
#define VERIFY_PARAM_BAT_LEVEL(param) (((param) > MAX_PARAM_BAT_LEVEL) ? NRF_ERROR_INVALID_PARAM : NRF_SUCCESS);

/**@brief Check if valid volume (0% - 100%)
 * @return If invalid, return NRF_ERROR_INVALID_PARAM
 */
#define MIN_PARAM_VOLUME		0
#define MAX_PARAM_VOLUME		100
//#define VERIFY_PARAM_VOLUME(param)									\
//if ((param) > MAX_PARAM_VOLUME) {									\
//	NRF_LOG_DEBUG("ERROR. Tried to set invalid volume: %d%%\r\n", (param));				\
//	return NRF_ERROR_INVALID_PARAM;									\
//} else {												\
//	return NRF_SUCCESS;										\
//}
#define VERIFY_PARAM_VOLUME(param) (((param) > MAX_PARAM_VOLUME) ? NRF_ERROR_INVALID_PARAM : NRF_SUCCESS);

/**@brief Check if valid TX power level for nRF52832 SoC
 * @brief (accepted values are -40, -20, -16, -12, -8, -4, 0, 3, and 4 dBm)
 * @return If invalid, return NRF_ERROR_INVALID_PARAM
 */
#define VERIFY_PARAM_TX_POWER(param)									\
if ( (((param) == RADIO_TXPOWER_TXPOWER_Pos4dBm)	||						\
     ((param) == RADIO_TXPOWER_TXPOWER_Pos3dBm) 	||						\
     ((param) == RADIO_TXPOWER_TXPOWER_0dBm)		||						\
     ((param) == RADIO_TXPOWER_TXPOWER_Neg4dBm)		||						\
     ((param) == RADIO_TXPOWER_TXPOWER_Neg8dBm)		||						\
     ((param) == RADIO_TXPOWER_TXPOWER_Neg12dBm)	||						\
     ((param) == RADIO_TXPOWER_TXPOWER_Neg16dBm)	||						\
     ((param) == RADIO_TXPOWER_TXPOWER_Neg20dBm)	||						\
     ((param) == RADIO_TXPOWER_TXPOWER_Neg40dBm)) == false ) {						\
    NRF_LOG_DEBUG("ERROR. Tried to set invalid TX power: %d dBm\r\n", (param));				\
    return NRF_ERROR_INVALID_PARAM;									\
}

/**@brief Check if the error code is equal to NRF_SUCCESS, if not return the error code.
 */
#define RETURN_IF_ERROR(err_code)									\
if ((err_code) != NRF_SUCCESS) {									\
    NRF_LOG_WARNING("Err code returned in file: %s, line: %d, code %d \r\n",				\
    nrf_log_push(__FILE__), __LINE__, err_code);							\
    return (err_code);											\
}

/**@brief Check if the input pointer is NULL, if so it returns NRF_ERROR_NULL.
 */
#define NULL_PARAM_CHECK(param)         \
if ((param) == NULL) {                  \
    return NRF_ERROR_NULL;		\
}

#endif //EG_COMMON_H_
