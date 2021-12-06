#ifndef EG_FLASH_H_
#define EG_FLASH_H_

/*
 * Driver for the EllieGrid nrf52 internal flash
 * Handles saving/fetching user preferences, alarm/compliance data
 */

#include "bq2409x.h"
#include "eg_common.h"
#include "eg_leds.h"
#include "fds.h"
#include "nrf_pwr_mgmt.h"

#if defined(BOARD_EG_v0_4) || defined(BOARD_EG_v1_1) || defined(BOARD_EG_v1_2)
    #include "elliegrid_v1_x.h"
#else
#error "BOARD IS NOT DEFINED"
#endif

#define BYTE_LENGTH_BLE_PACKET      MAX_BLE_PACKET_LENGTH
#define CHUNK_LENGTH_BLE_PACKET     BYTES_TO_WORDS(BYTE_LENGTH_BLE_PACKET)  // 5 words
#define CHUNK_LENGTH_ALARM_DATA     CHUNK_LENGTH_BLE_PACKET * 3             // 15 words

#define FILE_ID_USER_PREFS          0x0002
#define KEY_USER_PREFS_GET          0x0
#define KEY_USER_PREFS_SET          0x1
// NOTE: Don't change any REC_KEY values, as it'll break backwards compatibility with older firmware
enum {
    REC_KEY_LED_R = 0x0001,
    REC_KEY_LED_G,
    REC_KEY_LED_B,
    REC_KEY_LED_PATTERN,
    REC_KEY_TX_POWER,
    REC_KEY_CHAR_DURATION_MS,
    REC_KEY_INTER_CHAR_DURATION_MS,
    REC_KEY_SPACE_DURATION_MS,
};

#define FILE_ID_PB_STATE            0x0003
enum {
    REC_KEY_TX_POWER_DEPR = 0x0001,
    REC_KEY_DEVICE_NAME,
    REC_KEY_DEVICE_NAME_LEN
};

#define FILE_ID_MAINTAIN_DATA 0x0005
enum {
    REC_KEY_SERIAL_NUM = 0x0001,
};

// Misc. Defaults
#define DEFAULT_DEVICE_NAME         "Regina's Wall"
#define DEFAULT_DEVICE_NAME_LEN     13
#define DEFAULT_SERIAL_NUM          "STW-01-XXXX"
#define DEFAULT_SERIAL_NUM_LEN      11
#define MAX_DEVICE_NAME_LENGTH      MAX_BLE_PACKET_LENGTH	// 17=20-byte packet-3 for header/length (5 flash words) //can support up to len=26 (26+2)+3 =31 bytes in scan response packet
#define MAX_CHUNK_LENGTH            CHUNK_LENGTH_ALARM_DATA	// Biggest chunk of data used (alarm data)
#define GC_CONTIG_THRESHOLD         MAX_CHUNK_LENGTH * 10	// Arbitrary buffer size to prevent exceeding available flash space

extern volatile uint8_t fds_write_flag;
extern uint8_t g_device_name[];
extern uint8_t g_device_name_len;
extern uint8_t g_serial_num[];
extern int8_t g_tx_power_level;
extern uint8_t g_led_r;
extern uint8_t g_led_g;
extern uint8_t g_led_b;
extern uint8_t g_led_pattern_sel;
extern uint32_t g_char_duration_ms;
extern uint32_t g_inter_char_duration_ms;
extern uint32_t g_space_duration_ms;

/**
 * @brief Custom fds_evt_handler
 * @param fds_evt_t const * const
 */
void eg_fds_evt_handler(fds_evt_t const * const p_fds_evt);

/**
 * @brief Initializes FDS
 */
void eg_fds_init(void);

/**
 * @brief Prints all records in FDS
 */
void eg_fds_read_all(void);

/**
 * @brief Read data in record
 * @param FILE_ID, REC_KEY
 */
void eg_fds_read_data(uint16_t file_id, uint16_t rec_key);

/**
 * @brief eg_fds_write_data
 * @param FILE_ID, REC_KEY, *data, length (in bytes)
 */
void eg_fds_write_data(uint16_t fid, uint16_t key, void const * p_data, uint32_t len);

/**
 * @brief eg_fds_update_data
 * @param FILE_ID, REC_KEY, *data, length (in bytes)
 */
void eg_fds_update_data(uint16_t fid, uint16_t key, void const * p_data, uint32_t len);

/**
 * @brief eg_fds_update_user_data_string
 * @param FILE_ID, REC_KEY, global variable, new string value, length of string (# characters)
 */
void eg_fds_update_user_data_string(uint16_t file_id, uint16_t rec_key, uint8_t *g_var, uint8_t *new_val, uint8_t len);
ret_code_t eg_fds_update_user_data_byte(uint16_t file_id, uint16_t rec_key, uint8_t *var, uint32_t new_val);
ret_code_t eg_fds_update_user_data_word(uint16_t file_id, uint16_t rec_key, uint32_t *var, uint32_t new_val);

/**
 * @brief Delete (invalidate) a record
 * @param FILE_ID, REC_KEY
 */
void eg_fds_delete_record(uint16_t file_id, uint16_t rec_key);

/**
 * @brief Garbage collection
 * @param FILE_ID, REC_KEY
 */
void eg_fds_gc(void);
void eg_fds_free_words(void);
void eg_fds_stat(void);
void eg_fds_delete_files(void);

ret_code_t eg_fds_record_exists(uint16_t fid, uint16_t key);
ret_code_t eg_fds_fetch_user_prefs(void);
ret_code_t eg_fds_fetch_user_data_byte(uint16_t file_id, uint16_t rec_key, uint8_t *var, uint32_t default_val);
ret_code_t eg_fds_check_default_user_data_byte(uint16_t file_id, uint16_t rec_key, uint8_t *var, uint32_t default_val);
ret_code_t eg_fds_fetch_user_data_word(uint16_t file_id, uint16_t rec_key, uint32_t *var, uint32_t default_val);
ret_code_t eg_fds_fetch_data_string(uint16_t file_id, uint16_t rec_key, uint8_t *var, uint8_t *default_val, uint8_t var_len);
ret_code_t eg_fds_check_default_user_data_word(uint16_t file_id, uint16_t rec_key, uint32_t *var, uint32_t default_val);

#endif //EG_FLASH_H_
