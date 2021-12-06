#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "app_button.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "app_timer.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_bas.h"
#include "ble_dfu.h"
#include "ble_dis.h"
#include "ble_egs.h"
#include "ble_nus.h"
#include "fds.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_bootloader_info.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_i2s.h"
#include "nrf_drv_power.h"
#include "nrf_drv_ppi.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"
#include "nrf_power.h"
#include "nrf_pwr_mgmt.h"
#include "nrfx_gpiote.h"

#include "elliegrid_v1_x.h"
#include "advertiser_beacon.h"
#include "ah1893.h"
#include "ble_tcs.h"
#include "bq2409x.h"
#include "dio6305b.h"
#include "eg_common.h"
#include "eg_flash.h"
#include "eg_leds.h"
#include "eg_rtc.h"
#include "eg_twi.h"
#include "lc709203.h"
#include "mtch101.h"
#include "sk6812.h"
#include "sk6812_i2s.h"


/**< APP_ADV_INTERVAL Range: 20ms to 10.24 seconds, in steps of 0.625ms */
/**< APP_ADV_DURATION Range: 20ms to 10.24 seconds, in steps of 0.625ms */
// Fast advertising at 20ms for 30s recommended in Apple BLE guidelines for initial discovery
// Slow advertising at 211.25ms (shorter interval recommended by Apple)
// TODO: fine tune the slow advertising interval (longer = better battery life)
#define FAST_APP_ADV_INTERVAL		MSEC_TO_UNITS(20, UNIT_0_625_MS)	    // 20ms; Range: 20ms to 10.24 seconds, in steps of 0.625m
#define FAST_APP_ADV_DURATION           3000                                        /**< The advertising duration (30 seconds) in units of 10 milliseconds. */
#define SLOW_APP_ADV_INTERVAL		MSEC_TO_UNITS(1285, UNIT_0_625_MS)	    // 1285ms; longest interval recommended by Apple (saves power)
//#define SLOW_APP_ADV_INTERVAL		MSEC_TO_UNITS(211.25, UNIT_0_625_MS)	    // 152.5, 211.25, 318.75, 417.5, 546.25, 760, 852.5, 1022.5, 1285
#define SLOW_APP_ADV_DURATION		BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED       /**< ABA: always be advertising. */
#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

/**< MIN_CONN_INTERVAL = 100ms for low priority; Range: 7.5ms to 4000ms, in 1.25ms units */
/**< MAX_CONN_INTERVAL = 200ms for low priority; Range: 7.5ms to 4000ms, in 1.25ms units */
/**< SLAVE_LATENCY = 0 ignored packets for best responsiveness; Range 0x0000 to 0x1F3(499) */
/**< CONN_SUP_TIMEOUT = 4 seconds; Range: 100ms to 32s, in 10ms units */
//#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(50, UNIT_1_25_MS)		    /**< Minimum acceptable connection interval (50 ms). */
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (100 ms). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (200 ms). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */


// Beacon Params
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(100, UNIT_0_625_MS)  /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */
#define APP_BEACON_INFO_LENGTH          0x17                               /**< Total length of information advertised by the Beacon. */
#define APP_ADV_DATA_LENGTH             0x15                               /**< Length of manufacturer specific data in the advertisement. */
#define APP_DEVICE_TYPE                 0x02                               /**< 0x02 refers to Beacon. */
#define APP_MEASURED_RSSI               0xC4                               /**< The Beacon's measured RSSI at 1 meter distance in dBm. [0xC4 = -60]*/
#define APP_COMPANY_IDENTIFIER          0x004C				   /**< Apple company identifier for iBeacon */
#define APP_MAJOR_VALUE                 0x0F, 0xA0                         /**< Major value used to identify Beacons. */
#define APP_MINOR_VALUE                 0x07, 0xD0                         /**< Minor value used to identify Beacons. */
#define APP_BEACON_UUID			0xE1, 0xBE, 0xAC, 0x05, \
					0x80, 0xD6, 0x42, 0xC2, \
					0xAA, 0x89, 0x94, 0x0F, \
					0x97, 0xC9, 0xBD, 0x04

#define HEAD_MESSAGE_0			0x53    // 'S'
#define HEAD_MESSAGE_1			0x54    // 'T'
#define HEAD_SOFT_RESET_0		0xE8
#define HEAD_SOFT_RESET_1		0x02
#define HEAD_DISCONNECT_0		0xE8
#define HEAD_DISCONNECT_1		0x08
#define HEAD_FACTORY_RESET_0		0xDE
#define HEAD_FACTORY_RESET_1		0xAD
#define HEAD_USER_PREFS_0 		0x75	// 'u'
#define HEAD_USER_PREFS_1 		0x70	// 'p'
#define HEAD_DEVICE_NAME_0		0x64	// 'd'
#define HEAD_DEVICE_NAME_1		0x6E	// 'n'
#define RX_STR_LEN_OFFSET		0x20	// simplifies setting with nRF Connect; '*'=0x2A --> len=0xA=10
#define HEAD_SERIAL_NUM_0		0x73	// 's'
#define HEAD_SERIAL_NUM_1		0x6E	// 'n'
#define HEAD_DEBUG_0			0x64	// 'd'
#define HEAD_DEBUG_1			0x62	// 'b'

static ble_gap_adv_params_t m_adv_params;                                  /**< Parameters to be passed to the stack when starting advertising. */
static uint8_t              m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET; /**< Advertising handle used to identify an advertising set. */
static uint8_t              m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];  /**< Buffer for storing an encoded advertising set. */

/** Struct that contains pointers to the encoded advertising data. */
static ble_gap_adv_data_t m_adv_data = {
    .adv_data = {
        .p_data = m_enc_advdata,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX
    },
    .scan_rsp_data = {
        .p_data = NULL,
        .len    = 0
    }
};

/**< Information advertised by the Beacon. */
static uint8_t m_beacon_info[APP_BEACON_INFO_LENGTH] = {
    APP_DEVICE_TYPE,     // Manufacturer specific information. Specifies the device type in this implementation.
    APP_ADV_DATA_LENGTH, // Manufacturer specific information. Specifies the length of the manufacturer specific data in this implementation.
    APP_BEACON_UUID,     // 128 bit UUID value.
    APP_MAJOR_VALUE,     // Major arbitrary value that can be used to distinguish between Beacons.
    APP_MINOR_VALUE,     // Minor arbitrary value that can be used to distinguish between Beacons.
    APP_MEASURED_RSSI    // Manufacturer specific information. The Beacon's measured TX power in this implementation.
};

// Device Information
//#define DEVICE_NAME                     "Ellie-XXXX"                                /**< Name of device. Will be included in the advertising data. */
//#define SERIAL_NUMBER                   "EG7-01-XXXX"  //DEFAULT_SERIAL_NUM         // not actually used, read from flash
#define MANUFACTURER_NAME               "EllieGrid, Inc."                           /**< Manufacturer. Will be passed to Device Information Service. */
#define MODEL_NUMBER			"STW1"
#define HW_REVISION			"-.-.-"                                     // follow semantic versioning (major, minor, patch)
#define FW_REVISION			"0.0.1"
#define SW_REVISION                     "-.-.-"
#define ELLIEGRID_BLE_COMPANY_ID	0x04E2

#define BUTTON_DELAY_FAST		50
#define BUTTON_DELAY_SLOW		1000
#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(BUTTON_DELAY_SLOW)	    /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */
#define BATTERY_LEVEL_MEAS_INTERVAL     APP_TIMER_TICKS(30000)                      /**< Battery level measurement interval (ticks). //check battery life and update manuf_data every 30s*/

#define DEAD_BABE                       0xDEADBABE                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

// Globals for BLE-related
BLE_BAS_DEF(m_bas);                                                                 /**< Structure used to identify the battery service. */
BLE_EGS_DEF(m_egs);                                                                 /**< LED Button Service instance. */
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);                                   /**< BLE NUS service instance. */
NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */
uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;				    /**< Handle of the current connection. */
static uint16_t m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;              /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
uint8_t			g_manuf_data[6];					    // 6 bytes of manuf specific data in adv_data
int8_t			g_tx_power_level;					    // TX power level in adv_data
uint8_t			g_device_name[MAX_DEVICE_NAME_LENGTH];
uint8_t			g_device_name_len = DEFAULT_DEVICE_NAME_LEN;
uint8_t			g_serial_num[DEFAULT_SERIAL_NUM_LEN+1];			    // +1 for string null termination
bool                    g_client_valid;						    // flag used to determine if connected client is valid (sets the current time properly)
uint16_t                g_nus_max_data_len;					    // holds the value of m_ble_nus_max_data_len and updates every battery_level_update()

// Globals for pillbox state-related
volatile uint8_t 	fds_write_flag = 0;					    // indicates when FDS has finished writing to flash memory
uint8_t 		g_last_rsoc;						    // holds the last valid battery relative state of charge (RSOC)
time_t 			g_time_of_plug;						    // last epoch of when USB charger connected
time_t 			g_time_of_unplug;					    // last epoch of when USB charger disconnected
time_t 			g_time_of_plug_conn;					    // time (in secs) from the initial PGn, indicating USB connected and battery charging
uint8_t			g_notify_state;						    // holds the value to notify on COMMAND_STATUS characteristic   0x MAG-CAP
APP_TIMER_DEF(m_battery_timer_id);                                                  // battery timer

// Globals for alarm-related
time_t 			g_current_time;						    // used for alarm scheduling logic
uint8_t 		g_current_hour;						    // used for alarm scheduling logic
uint8_t 		g_current_min;						    // used for alarm scheduling logic

// Globals for user preferences
uint8_t 		g_led_r;						    // red value for alarm LED (0-255)
uint8_t 		g_led_g;						    // green value for alarm LED (0-255)
uint8_t 		g_led_b;						    // blue value for alarm LED (0-255)
uint8_t 		g_led_pattern_sel;				    // LED pattern selection for alarm success
int8_t                  g_byte_data_ack[3];					    // HEAD_DATA_0, HEAD_DATA_1, g_value (lp_mode_en, factory_rst)
uint8_t 		g_4byte_data_ack[6];					    // HEAD_DATA_0, HEAD_DATA_1, EPOCH x4 (CT setting [RTC epoch], alarm setting [comp | next epoch])
uint8_t 		g_user_prefs_ack[6];					    // HEAD_DATA_0, HEAD_DATA_1, KEY, GET/SET, g_value_MSB, g_value_LSB (volume)
uint32_t                g_char_duration_ms;
uint32_t                g_inter_char_duration_ms;
uint32_t                g_space_duration_ms;

volatile uint8_t 	g_i2s_data_sent_flag = 0;
sk6812_led_t		m_led_buffer_tx[NUM_LEDS];
uint32_t		m_i2s_led_buffer_tx[I2S_LEDS_FRAME_WORD_SIZE];


// Function prototypes to suppress compile warnings...
void advertising_init(void);
void advertising_start(void);
void eg_update_advdata(void);

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
#if NRF_LOG_ENABLED
    error_info_t * err_info = (error_info_t*)info;
    NRF_LOG_ERROR(" id = %d, pc = %d, file = %s, line number: %d, error code = %d = %s ",       \
		    id, pc, nrf_log_push((char*)err_info->p_file_name),				\
		    err_info->line_num, err_info->err_code,					\
		    nrf_log_push((char*)nrf_strerror_find(err_info->err_code)));
#endif
    
    // Illuminate solid red LED (not blinking) in top left corner to indicate app_error_fault
    eg_set_single_led(DISPLAY_ERROR_LED, RESET_LED_R, RESET_LED_G, RESET_LED_B, LED_DIN_PIN);
    
    // TODO: Enable system reset?
    // On assert, the system can only recover with a reset.
//#ifndef DEBUG
//    NVIC_SystemReset();
//#endif

    NRF_LOG_FINAL_FLUSH();
    nrf_delay_ms(5);
    app_error_save_and_stop(id, pc, info);
}


/**@brief Function for performing battery measurement and updating the Battery Level characteristic
 *        in Battery Service.
 */
static void battery_level_update(void) {
    uint32_t err_code;

    // Read the fuel gauge
    uint16_t rsoc;
    eg_twi_enable();
    fg_read_RSOC(&rsoc);
    eg_twi_disable();
    err_code = VERIFY_PARAM_BAT_LEVEL(rsoc);
    if (err_code == NRF_SUCCESS) {
	g_last_rsoc = (uint8_t) rsoc;
    }
    
    // Cross reference with cell voltage and profile lookup table?
    
    // Update battery level characteristic
    err_code = ble_bas_battery_level_update(&m_bas, g_last_rsoc, BLE_CONN_HANDLE_ALL);
    if ((err_code != NRF_SUCCESS) &&
	(err_code != NRF_ERROR_INVALID_STATE) &&
	(err_code != NRF_ERROR_RESOURCES) &&
	(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)) {
	APP_ERROR_HANDLER(err_code);
    }
    
    // Keep track of m_ble_nus_max_data_len; TODO: move to gatt_update_params
    if (m_ble_nus_max_data_len != g_nus_max_data_len) {
        g_nus_max_data_len = m_ble_nus_max_data_len;
        NRF_LOG_DEBUG(" --- setting g_nus_max_data_len: %d", g_nus_max_data_len);
    }
}


/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *
 * @param[in] p_context  Pointer used for passing some arbitrary information (context) from the
 *                       app_start_timer() call to the timeout handler.
 */
static void battery_level_meas_timeout_handler(void * p_context) {
    UNUSED_PARAMETER(p_context);
    battery_level_update();
}


/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_evt       Nordic UART Service event.
 */
/**@snippet [Handling the data received over BLE] */
static void nus_data_handler(ble_nus_evt_t * p_evt) {
    uint32_t err_code;
    uint16_t len;
    
    if (p_evt->type == BLE_NUS_EVT_RX_DATA) {
        uint8_t * p_data = (uint8_t *) p_evt->params.rx_data.p_data;
	uint16_t length = p_evt->params.rx_data.length;
	
	NRF_LOG_INFO("\n\nReceived %d bytes on NUS RX characteristic:", length);
	NRF_LOG_HEXDUMP_INFO(p_data, length);
//        NRF_LOG_INFO("Received data from BLE NUS. Writing data length: %d, %d", p_evt->params.rx_data.length, p_evt->params.rx_data.p_data[0]);
//        NRF_LOG_HEXDUMP_INFO(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);

	
	// Display message received
        if (p_data[0]==HEAD_MESSAGE_0 && p_data[1]==HEAD_MESSAGE_1) {
            for (uint8_t ii=0; ii < length-2; ii++) {
                char cc = p_data[ii+2];
                if (abc_validate(cc) == true) {
//                    NRF_LOG_INFO("%c", cc);
                    abc_display_char(cc);
                }
            }
            eg_leds_off();
        }

        // Handle Pillbox Debugging
	else if (p_data[0]==HEAD_DEBUG_0 && p_data[1]==HEAD_DEBUG_1) {
            // Easter Eggs from Stranger Things
            if (p_data[2] == '0') {
                stranger_things_easter_egg_0();
            }

            else if (p_data[2] == '1') {
                stranger_things_easter_egg_1();
            }

            else if (p_data[2] == '2') {
                regina_easter_egg();
            }
	    
	    // Print flash stats
	    else if (p_data[2] == '3') {
		eg_fds_stat();
	    }
		
	    // Run flash garbage collection
	    else if (p_data[2] == '4') {
		eg_fds_gc();
	    }

            else if (p_data[2] == '5') {
                rainbow_cycle(40);
                eg_leds_off();
            }

            else if (p_data[2] == '6') {
                theater_chase_rainbow(40);
                eg_leds_off();
            }

	    // Print user preference globals
	    else if (p_data[2] == '7') {
		NRF_LOG_INFO("USER PREFERENCES global values:");
		NRF_LOG_INFO("\t g_led_r: %d", g_led_r);
		NRF_LOG_INFO("\t g_led_g: %d", g_led_g);
		NRF_LOG_INFO("\t g_led_b: %d", g_led_b);
		NRF_LOG_INFO("\t g_led_pattern_sel: %d", g_led_pattern_sel);
                NRF_LOG_INFO("\t g_char_duration_ms: %d", g_char_duration_ms);
                NRF_LOG_INFO("\t g_inter_char_duration_ms: %d", g_inter_char_duration_ms);
                NRF_LOG_INFO("\t g_space_duration_ms: %d", g_space_duration_ms);
	    }

	    // Print and return plug time, charge time information
	    else if (p_data[2] == '8') {
		NRF_LOG_INFO("\t g_last_rsoc: %d%%", g_last_rsoc);
		NRF_LOG_INFO("\t g_time_of_plug: %d epoch", g_time_of_plug);
		NRF_LOG_INFO("\t g_time_of_unplug: %d epoch", g_time_of_unplug);
		NRF_LOG_INFO("\t g_time_of_plug_conn: %d seconds", g_time_of_plug_conn);
	    }

            else if (p_data[2] == 'O') {
                eg_leds_off();
            }
	}
	
	// Handle Device Name setting
	else if (p_data[0]==HEAD_DEVICE_NAME_0 && p_data[1]==HEAD_DEVICE_NAME_1) {
	    if (eg_validate_byte(p_data[2], MIN_PARAM_DN_CHAR, MAX_PARAM_DN_CHAR)) {
		// Get string length
		uint8_t str_len = p_data[2] - RX_STR_LEN_OFFSET;
		if (eg_validate_byte(str_len, MIN_PARAM_DN_LEN, MAX_PARAM_DN_LEN)) {
		    // Update flash
		    eg_fds_update_user_data_byte(FILE_ID_PB_STATE, REC_KEY_DEVICE_NAME_LEN, &g_device_name_len, (uint32_t) str_len);
		    eg_fds_update_user_data_string(FILE_ID_PB_STATE, REC_KEY_DEVICE_NAME, g_device_name, &p_data[3], g_device_name_len);
		    
		    // Change device name in advdata
		    eg_update_advdata();
		    
		    // DeviceName updated in char, but not in advertising data
		    // TODO: current solution is to issue a soft reset (from client command) to capture latest device name from flash
		}
	    }
	}
	
	// Handle Serial Number setting
	else if (p_data[0]==HEAD_SERIAL_NUM_0 && p_data[1]==HEAD_SERIAL_NUM_1) {
	    // Update serial number in flash
	    eg_fds_update_user_data_string(FILE_ID_MAINTAIN_DATA, REC_KEY_SERIAL_NUM, g_serial_num, &p_data[2], DEFAULT_SERIAL_NUM_LEN);
		
	    // TODO: Issue hard reset to show serial number change in char
	}
	
	// Handle getting/setting User Preferences
	else if (p_data[0]==HEAD_USER_PREFS_0 && p_data[1]==HEAD_USER_PREFS_1) {       
	    // Prep acknowledge notification
	    memset(&g_user_prefs_ack, 0, sizeof(g_user_prefs_ack));
	    g_user_prefs_ack[0] = HEAD_USER_PREFS_0;
	    g_user_prefs_ack[1] = HEAD_USER_PREFS_1;

	    switch (p_data[2]) {
		case REC_KEY_LED_R:
		    // Write data to flash, set global value
		    if (p_data[3] == KEY_USER_PREFS_SET) {
			g_user_prefs_ack[3] = KEY_USER_PREFS_SET;
			eg_fds_update_user_data_byte(FILE_ID_USER_PREFS, REC_KEY_LED_R, &g_led_r, p_data[4]);
			NRF_LOG_DEBUG("Set alarm LEDs to R/G/B: %d, %d, %d", g_led_r, g_led_g, g_led_b);
		    } else {
			g_user_prefs_ack[3] = KEY_USER_PREFS_GET;
			NRF_LOG_DEBUG("g_led_r/g/b: %d, %d, %d", g_led_r, g_led_g, g_led_b);
		    }

		    // Send acknowledgement
		    g_user_prefs_ack[2] = REC_KEY_LED_R;
		    g_user_prefs_ack[4] = g_led_r;
		    len = sizeof(g_user_prefs_ack);
		    err_code = ble_nus_data_send(&m_nus, g_user_prefs_ack, &len, m_conn_handle);
		    APP_ERROR_CHECK(err_code);
		    break;

		case REC_KEY_LED_G:
		    // Write data to flash, set global value
		    if (p_data[3] == KEY_USER_PREFS_SET) {
			g_user_prefs_ack[3] = KEY_USER_PREFS_SET;
			eg_fds_update_user_data_byte(FILE_ID_USER_PREFS, REC_KEY_LED_G, &g_led_g, p_data[4]);
			NRF_LOG_DEBUG("Set alarm LEDs to R/G/B: %d, %d, %d", g_led_r, g_led_g, g_led_b);
		    } else {
			g_user_prefs_ack[3] = KEY_USER_PREFS_GET;
			NRF_LOG_DEBUG("g_led_r/g/b: %d, %d, %d", g_led_r, g_led_g, g_led_b);
		    }

		    // Send acknowledgement
		    g_user_prefs_ack[2] = REC_KEY_LED_G;
		    g_user_prefs_ack[4] = g_led_g;
		    len = sizeof(g_user_prefs_ack);
		    err_code = ble_nus_data_send(&m_nus, g_user_prefs_ack, &len, m_conn_handle);
		    APP_ERROR_CHECK(err_code);
		    break;

		case REC_KEY_LED_B:
		    // Write data to flash, set global value
		    if (p_data[3] == KEY_USER_PREFS_SET) {
			g_user_prefs_ack[3] = KEY_USER_PREFS_SET;
			eg_fds_update_user_data_byte(FILE_ID_USER_PREFS, REC_KEY_LED_B, &g_led_b, p_data[4]);
			NRF_LOG_DEBUG("Set alarm LEDs to R/G/B: %d, %d, %d", g_led_r, g_led_g, g_led_b);
		    } else {
			g_user_prefs_ack[3] = KEY_USER_PREFS_GET;
			NRF_LOG_DEBUG("g_led_r/g/b: %d, %d, %d", g_led_r, g_led_g, g_led_b);
		    }

		    // Send acknowledgement
		    g_user_prefs_ack[2] = REC_KEY_LED_B;
		    g_user_prefs_ack[4] = g_led_b;
		    len = sizeof(g_user_prefs_ack);
		    err_code = ble_nus_data_send(&m_nus, g_user_prefs_ack, &len, m_conn_handle);
		    APP_ERROR_CHECK(err_code);
		    break;

		case REC_KEY_LED_PATTERN:
		    // Write data to flash, set global value
		    if (p_data[3] == KEY_USER_PREFS_SET) {
			g_user_prefs_ack[3] = KEY_USER_PREFS_SET;
			eg_fds_update_user_data_byte(FILE_ID_USER_PREFS, REC_KEY_LED_PATTERN, &g_led_pattern_sel, p_data[4]);
			NRF_LOG_DEBUG("Set g_led_pattern_sel: %d", g_led_pattern_sel);
		    } else {
			g_user_prefs_ack[3] = KEY_USER_PREFS_GET;
			NRF_LOG_DEBUG("g_led_pattern_sel: %d", g_led_pattern_sel);
		    }

		    // Send acknowledgement
		    g_user_prefs_ack[2] = REC_KEY_LED_PATTERN;
		    g_user_prefs_ack[4] = g_led_pattern_sel;
		    len = sizeof(g_user_prefs_ack);
		    err_code = ble_nus_data_send(&m_nus, g_user_prefs_ack, &len, m_conn_handle);
		    APP_ERROR_CHECK(err_code);
		    break;
		    
		// Changing the TX Power will require a soft reset to re-init adv?
		case REC_KEY_TX_POWER:
		    // Write data to flash, set global value
		    if (p_data[3] == KEY_USER_PREFS_SET) {
			g_user_prefs_ack[3] = KEY_USER_PREFS_SET;
			eg_fds_update_user_data_byte(FILE_ID_USER_PREFS, REC_KEY_TX_POWER, &g_tx_power_level, p_data[4]);
			NRF_LOG_DEBUG("[Set] g_tx_power_level: %d", g_tx_power_level);
			
			// If receiving this user pref set, device isn't currently advertising so we can explicitly set tx_power
//			advertising_init();
//			err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_advertising.adv_handle, g_tx_power_level);
//			APP_ERROR_CHECK(err_code);
//			advertising_start();
		    } else {
			g_user_prefs_ack[3] = KEY_USER_PREFS_GET;
			NRF_LOG_DEBUG("g_tx_power_level: %d", g_tx_power_level);
		    }

		    // Send acknowledgement
		    g_user_prefs_ack[2] = REC_KEY_TX_POWER;
		    g_user_prefs_ack[4] = g_tx_power_level;
		    len = sizeof(g_user_prefs_ack);
		    err_code = ble_nus_data_send(&m_nus, g_user_prefs_ack, &len, m_conn_handle);
		    APP_ERROR_CHECK(err_code);
		    break;

		case REC_KEY_CHAR_DURATION_MS:
		    // Write data to flash, set global value
		    if (p_data[3] == KEY_USER_PREFS_SET) {
			g_user_prefs_ack[3] = KEY_USER_PREFS_SET;
			uint32_t data = (p_data[4] << 24) | (p_data[5] << 16) | (p_data[6] << 8) | p_data[7];
			err_code = eg_fds_update_user_data_word(FILE_ID_USER_PREFS, REC_KEY_CHAR_DURATION_MS, (uint32_t *) &g_char_duration_ms, data);
			APP_ERROR_CHECK(err_code);
			NRF_LOG_DEBUG("Set g_char_duration_ms: %d", g_char_duration_ms);
		    } else {
			g_user_prefs_ack[3] = KEY_USER_PREFS_GET;
			NRF_LOG_DEBUG("g_char_duration_ms: %d", g_char_duration_ms);
		    }

		    // Send acknowledgement
		    g_user_prefs_ack[2] = REC_KEY_CHAR_DURATION_MS;
		    g_user_prefs_ack[4] = MSB_16(g_char_duration_ms);
		    g_user_prefs_ack[5] = LSB_16(g_char_duration_ms);
		    len = sizeof(g_user_prefs_ack);
		    err_code = ble_nus_data_send(&m_nus, g_user_prefs_ack, &len, m_conn_handle);
		    APP_ERROR_CHECK(err_code);
		    break;

		case REC_KEY_INTER_CHAR_DURATION_MS:
		    // Write data to flash, set global value
		    if (p_data[3] == KEY_USER_PREFS_SET) {
			g_user_prefs_ack[3] = KEY_USER_PREFS_SET;
			uint32_t data = (p_data[4] << 24) | (p_data[5] << 16) | (p_data[6] << 8) | p_data[7];
			err_code = eg_fds_update_user_data_word(FILE_ID_USER_PREFS, REC_KEY_INTER_CHAR_DURATION_MS, (uint32_t *) &g_inter_char_duration_ms, data);
			APP_ERROR_CHECK(err_code);
			NRF_LOG_DEBUG("Set g_inter_char_duration_ms: %d", g_inter_char_duration_ms);
		    } else {
			g_user_prefs_ack[3] = KEY_USER_PREFS_GET;
			NRF_LOG_DEBUG("g_inter_char_duration_ms: %d", g_inter_char_duration_ms);
		    }

		    // Send acknowledgement
		    g_user_prefs_ack[2] = REC_KEY_INTER_CHAR_DURATION_MS;
		    g_user_prefs_ack[4] = MSB_16(g_inter_char_duration_ms);
		    g_user_prefs_ack[5] = LSB_16(g_inter_char_duration_ms);
		    len = sizeof(g_user_prefs_ack);
		    err_code = ble_nus_data_send(&m_nus, g_user_prefs_ack, &len, m_conn_handle);
		    APP_ERROR_CHECK(err_code);
		    break;

		case REC_KEY_SPACE_DURATION_MS:
		    // Write data to flash, set global value
		    if (p_data[3] == KEY_USER_PREFS_SET) {
			g_user_prefs_ack[3] = KEY_USER_PREFS_SET;
			uint32_t data = (p_data[4] << 24) | (p_data[5] << 16) | (p_data[6] << 8) | p_data[7];
			err_code = eg_fds_update_user_data_word(FILE_ID_USER_PREFS, REC_KEY_SPACE_DURATION_MS, (uint32_t *) &g_space_duration_ms, data);
			APP_ERROR_CHECK(err_code);
			NRF_LOG_DEBUG("Set g_space_duration_ms: %d", g_space_duration_ms);
		    } else {
			g_user_prefs_ack[3] = KEY_USER_PREFS_GET;
			NRF_LOG_DEBUG("g_space_duration_ms: %d", g_space_duration_ms);
		    }

		    // Send acknowledgement
		    g_user_prefs_ack[2] = REC_KEY_SPACE_DURATION_MS;
		    g_user_prefs_ack[4] = MSB_16(g_space_duration_ms);
		    g_user_prefs_ack[5] = LSB_16(g_space_duration_ms);
		    len = sizeof(g_user_prefs_ack);
		    err_code = ble_nus_data_send(&m_nus, g_user_prefs_ack, &len, m_conn_handle);
		    APP_ERROR_CHECK(err_code);
		    break;

		default:
		    // Send fail byte 0xFF
		    g_user_prefs_ack[2] = 0xFF;
		    len = sizeof(g_user_prefs_ack);
		    err_code = ble_nus_data_send(&m_nus, g_user_prefs_ack, &len, m_conn_handle);
		    APP_ERROR_CHECK(err_code);
		    break;
	    }
	}
	
	// Handle Factory Reset command
	else if (p_data[0]==HEAD_FACTORY_RESET_0 && p_data[1]==HEAD_FACTORY_RESET_1) {
	    if (p_data[2] == 0x01) {
		NRF_LOG_INFO("!!! RESET TO FACTORY SETTINGS !!!");

		// Delete flash data (alarms, user prefs)
		eg_fds_delete_files();

		// Run garbage collection
		eg_fds_gc();

		// Wait until flash GC finish
		// Fetch default flash data
//		err_code = eg_fds_fetch_user_prefs();
//		err_code = eg_fds_fetch_alarms();

		// Issue soft reset to restore defaults? TODO need to timeout until after flash tasks
//		sd_nvic_SystemReset();

		// Currently, user must press the reset button manually to restore defaults
	    }
        
	    // Send factory reset acknowledgement
	    g_byte_data_ack[0] = HEAD_FACTORY_RESET_0;
	    g_byte_data_ack[1] = HEAD_FACTORY_RESET_1;
	    g_byte_data_ack[2] = p_data[2];
	    len = sizeof(g_byte_data_ack);
	    err_code = ble_nus_data_send(&m_nus, g_byte_data_ack, &len, m_conn_handle);
	    APP_ERROR_CHECK(err_code);
	}

	// Handle soft reset command
	else if (p_data[0]==HEAD_SOFT_RESET_0 && p_data[1]==HEAD_SOFT_RESET_1) {
	    if (p_data[2] == 0x01) {
		sd_nvic_SystemReset();
	    }
	}

	// Handle disconnect command
	else if (p_data[0]==HEAD_DISCONNECT_0 && p_data[1]==HEAD_DISCONNECT_1) {
	    err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
	    APP_ERROR_CHECK(err_code);
	}
    }
}


/**@brief Function for handling write events to the LED characteristic.
 *
 * @param[in] p_egs     Instance of LED Button Service to which the write applies.
 * @param[in] led_state Written/desired state of the LED.
 */
static void egs_led_write_handler(uint16_t conn_handle, ble_egs_t * p_egs, uint32_t led_state) {
    uint32_t temp;		    //used for maintaining LED state
    uint8_t select_led_comp;	    //represents compartment to display LED animation
    uint32_t select_led_pattern;    //represents LED animation pattern
    uint32_t command_input = led_state >> 28;
    
    NRF_LOG_DEBUG(">>> egs_led_write_handler: 0x%08x (%d)", led_state, command_input);
    
    switch (command_input) {
        case COMMAND_LEDS:
            if (led_state & 0x0FFFFFFF) {
                if (g_led_r || g_led_g || g_led_b) {
                    // Illuminate addressable LEDs
                    drive_app_compartment_leds(led_state, g_led_r, g_led_g, g_led_b);
                }
            } else {
                eg_leds_off();
            }
            break;
        
        case COMMAND_SELECT_LEDS:
            // Display LED animation on selected compartment (loading animation)
            select_led_comp = 0;
            select_led_pattern = 0;
            
            if (led_state & 0x0FFFFFFF) {
                if (g_led_r || g_led_g || g_led_b) {
                    // Animate selected compartment LEDs
                    for(uint8_t ii=0; ii<NUM_COMPARTMENTS; ii++) {
                        select_led_pattern = (led_state >> (ii*4)) & 0xF;
                        if (select_led_pattern) {
                            select_led_comp = ii+1;
                            break;
                        }
                    }
                    eg_leds_animate_select(select_led_comp, (uint8_t) select_led_pattern);
                }
            } else {
                // Stop LED animation on selected compartment
                eg_leds_off();
            }
            break;
        
        default:
            break;
    }
}


/**@brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press/release).
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action) {
    uint32_t err_code;

    switch (pin_no) {
	case CAP_OUTn_PIN:  // same as CAP_OUT_PIN
	    
	    NRF_LOG_DEBUG("button_event_handler() CAPTOUCH SENSE: %d", button_action);
	    
	    if (button_action == APP_BUTTON_PUSH) {
		// Send button press notification
		g_notify_state |= STATUS_BTN_PRESSED_MASK;
		ble_egs_on_button_change(m_conn_handle, &m_egs, g_notify_state);

		regina_easter_egg();

	    } else {    // (button_action == APP_BUTTON_RELEASE)	
		// Send button unpress notification
		g_notify_state &= ~STATUS_BTN_PRESSED_MASK;
		ble_egs_on_button_change(m_conn_handle, &m_egs, g_notify_state);
	    }
	    break;

	default:
	    break;
    }
}


/**@brief Function for initializing the button handler module.
 */
static void buttons_init(void) {
    uint32_t err_code;

    //The array must be static because a pointer to it will be saved in the button handler module.
    static app_button_cfg_t buttons[] = {
	{ CAP_OUTn_PIN, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, button_event_handler }
    };

    err_code = app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
}


/**@snippet [Handling the data received over BLE] */


/**@brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at a 1 second interval
 *          until the function returns true. When the function returns true, it means that the
 *          app is ready to reset to DFU mode.
 *
 * @param[in]   event   Power manager event.
 *
 * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
 */
static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event) {
    switch (event) {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            NRF_LOG_INFO("Power management wants to reset to DFU mode.");
            // YOUR_JOB: Get ready to reset into DFU mode
            //
            // If you aren't finished with any ongoing tasks, return "false" to
            // signal to the system that reset is impossible at this stage.
            //
            // Here is an example using a variable to delay resetting the device.
            //
            // if (!m_ready_for_reset)
            // {
            //      return false;
            // }
            // else
            //{
            //
            //    // Device ready to enter
            //    uint32_t err_code;
            //    err_code = sd_softdevice_disable();
            //    APP_ERROR_CHECK(err_code);
            //    err_code = app_timer_stop_all();
            //    APP_ERROR_CHECK(err_code);
            //}
            break;

        default:
            // YOUR_JOB: Implement any of the other events available from the power management module:
            //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
            //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
            //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
            return true;
    }

    NRF_LOG_INFO("Power management allowed to reset to DFU mode.");
    return true;
}

//lint -esym(528, m_app_shutdown_handler)
/**@brief Register application shutdown handler with priority 0.
 */
NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);


static void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context) {
    if (state == NRF_SDH_EVT_STATE_DISABLED) {
        // Softdevice was disabled before going into reset. Inform bootloader to skip CRC on next boot.
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);

        //Go to system off.
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}

/* nrf_sdh state observer. */
NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) = {
    .handler = buttonless_dfu_sdh_state_observer,
};

// YOUR_JOB: Update this code if you want to do anything given a DFU event (optional).
/**@brief Function for handling dfu events from the Buttonless Secure DFU service
 *
 * @param[in]   event   Event from the Buttonless Secure DFU service.
 */
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event) {
    switch (event) {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
            NRF_LOG_INFO("Device is preparing to enter bootloader mode.");
            // YOUR_JOB: Disconnect all bonded devices that currently are connected.
            //           This is required to receive a service changed indication
            //           on bootup after a successful (or aborted) Device Firmware Update.
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
            NRF_LOG_INFO("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            NRF_LOG_ERROR("Request to enter bootloader mode failed asynchroneously.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
	    APP_ERROR_CHECK(NRF_ERROR_INTERNAL);
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            NRF_LOG_ERROR("Request to send a response to client failed.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
//	    APP_ERROR_CHECK(NRF_ERROR_INTERNAL);
            break;

        default:
            NRF_LOG_ERROR("Unknown event from ble_dfu_buttonless.");
            break;
    }
}


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
 // TODO: handle assert error in final product (illuminate red LED?)
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name) {
     eg_set_single_led(DISPLAY_ERROR_LED + 4, RESET_LED_R, RESET_LED_G, RESET_LED_B, LED_DIN_PIN);
    app_error_handler(DEAD_BABE, line_num, p_file_name);
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void) {
    // Initialize timer module.
    uint32_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers.
    err_code = app_timer_create(&m_battery_timer_id, APP_TIMER_MODE_REPEATED, battery_level_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void) {
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, g_device_name, g_device_name_len);
    APP_ERROR_CHECK(err_code);

//   err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_COMPUTER);
//   APP_ERROR_CHECK(err_code);
    
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));
    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void) {
    uint32_t                  err_code;
    nrf_ble_qwr_init_t        qwr_init  = {0};
    ble_dis_init_t            dis_init;
    ble_bas_init_t            bas_init;
    ble_egs_init_t            egs_init;
    ble_nus_init_t            nus_init;
    ble_dfu_buttonless_init_t dfus_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;
    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));
    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, (char *)MANUFACTURER_NAME);
    ble_srv_ascii_to_utf8(&dis_init.model_num_str, (char *)MODEL_NUMBER);
//    ble_srv_ascii_to_utf8(&dis_init.serial_num_str, (char *)SERIAL_NUMBER);
    ble_srv_ascii_to_utf8(&dis_init.serial_num_str, (char *)g_serial_num);
    ble_srv_ascii_to_utf8(&dis_init.hw_rev_str, (char *)HW_REVISION);
    ble_srv_ascii_to_utf8(&dis_init.fw_rev_str, (char *)FW_REVISION);
    ble_srv_ascii_to_utf8(&dis_init.sw_rev_str, (char *)SW_REVISION);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);
    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Battery Service.
    memset(&bas_init, 0, sizeof(bas_init));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init.battery_level_char_attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_report_read_perm);
    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 0;
    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);

    // Initialize EllieGrid UART Service (NUS).
    memset(&nus_init, 0, sizeof(nus_init));
    nus_init.data_handler = nus_data_handler;
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);

    // Initialize EllieGrid Command Service
    egs_init.comp_led_write_handler = egs_led_write_handler;
    err_code = ble_egs_init(&m_egs, &egs_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Device Firmware Update Service.
    // Initialize the async SVCI interface to bootloader.
    err_code = ble_dfu_buttonless_async_svci_init();
    APP_ERROR_CHECK(err_code);
    dfus_init.evt_handler = ble_dfu_evt_handler;
    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);

    // TODO: init Current Time Service, Bond Management Service?
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt) {
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void) {
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));
    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting timers.
 */
static void application_timers_start(void) {
    APP_ERROR_CHECK(app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL));
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt) {
    uint32_t err_code;

    switch (ble_adv_evt) {
//        case BLE_ADV_EVT_FAST:
//            break;
//
//        case BLE_ADV_EVT_IDLE:
//            break;
        default:
            break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context) {
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected.");
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);

            // Stop advertising beacon on connection
//            app_beacon_stop();
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected.");
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
	    
	    // Disable and turn off LEDs on disconnect
            eg_leds_off();

            // Start advertising beacon on disconnection
//            app_beacon_start();
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            NRF_LOG_DEBUG("Pairing not supported.");
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            NRF_LOG_DEBUG("No system attributes have been stored.");
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void) {
    uint32_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);

    // Register a handler for SOC events (beacon timeslot)
    NRF_SDH_SOC_OBSERVER(m_soc_observer, 0, app_beacon_on_sys_evt, NULL);
}


/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt) {
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)) {
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }
    NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
                  p_gatt->att_mtu_desired_central,
                  p_gatt->att_mtu_desired_periph);
}


/**@brief   Function for initializing the GATT module.
 * @details The GATT module handles ATT_MTU and Data Length update procedures automatically.
 */
static void gatt_init(void) {
    // Comment out to prevent GATT_CONN_TIMEOUT on Nexus 6 --> phones that support BLE 4.0 and/or BLE 4.1 only
    // https://devzone.nordicsemi.com/f/nordic-q-a/22055/can-not-connect-to-devices-with-sdk-13-from-android-4-4-4
//    uint32_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
//    APP_ERROR_CHECK(err_code);

    // Added for NUS
    uint32_t err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 */
void advertising_init(void) {
    uint32_t               err_code;
    ble_advertising_init_t init;

    static ble_uuid_t adv_uuids[] = {{BLE_UUID_NUS_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN}};

    // Define manufacturing data
    ble_advdata_manuf_data_t manuf_data;
    manuf_data.company_identifier = ELLIEGRID_BLE_COMPANY_ID;
    manuf_data.data.p_data = g_manuf_data;
    manuf_data.data.size = sizeof(g_manuf_data);

    // Build advertising data struct to pass into @ref ble_advertising_init
    // ADV DATA [max_length = 31 bytes]
    // Flag field (length(1) + type(1) + flag(1) = 3 bytes)
    // 16-bit Service Class UUID ( length(1) + type(1) + UUID(2) = 4 bytes)
    // 128-bit Service Class UUID ( length(1) + type(1) + UUID(16) = 18 bytes)
    // Manufacturer Specific (length(1) + type(1) + companyID(2) + data(6) = 10 bytes)
    memset(&init, 0, sizeof(init));
    init.advdata.name_type               = BLE_ADVDATA_NO_NAME;
    init.advdata.include_appearance      = false;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_more_available.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    init.advdata.uuids_more_available.p_uuids = adv_uuids;
    init.advdata.p_manuf_specific_data = &manuf_data;

    // Build scan response data
    // SCAN RESPONSE [max_length = 31 bytes]
    // Device Name (length(1) + type(1) + name(20) = 22 bytes)
    // TX Power (length(1) + type(1) + power(1) = 3 bytes)
    init.srdata.name_type = BLE_ADVDATA_FULL_NAME;
    init.srdata.p_tx_power_level = &g_tx_power_level;

    // Set advertising parameters
    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = FAST_APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = FAST_APP_ADV_DURATION;	
    init.config.ble_adv_slow_enabled  = true;
    init.config.ble_adv_slow_interval = SLOW_APP_ADV_INTERVAL;
    init.config.ble_adv_slow_timeout  = SLOW_APP_ADV_DURATION;
    init.evt_handler = on_adv_evt;
    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

void beacon_advertising_init(void) {
    uint32_t      err_code;
    ble_advdata_t advdata;
    uint8_t       flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;

    ble_advdata_manuf_data_t manuf_specific_data;

    manuf_specific_data.company_identifier = APP_COMPANY_IDENTIFIER;
    manuf_specific_data.data.p_data = (uint8_t *) m_beacon_info;
    manuf_specific_data.data.size   = APP_BEACON_INFO_LENGTH;

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type             = BLE_ADVDATA_NO_NAME;
    advdata.flags                 = flags;
    advdata.p_manuf_specific_data = &manuf_specific_data;

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
    m_adv_params.p_peer_addr     = NULL;    // Undirected advertisement.
    m_adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval        = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.duration        = 0;       // Never time out.

    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &m_adv_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
void advertising_start(void) {
    uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}

void beacon_advertising_start(void) {
    ret_code_t err_code;
    err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the beacon timeslot functionality.
 */
static void beacon_advertiser_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}

uint32_t timeslot_init(void) {
    uint32_t err_code;
    static ble_beacon_init_t beacon_init;

    beacon_init.adv_interval  = 350;  //100;  //NON_CONNECTABLE_ADV_INTERVAL;
    beacon_init.p_data = (uint8_t *) m_beacon_info;
    beacon_init.data_size   = APP_BEACON_INFO_LENGTH;
    beacon_init.error_handler = beacon_advertiser_error_handler;

    err_code = sd_ble_gap_addr_get(&beacon_init.beacon_addr);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    // Increment device address by 2 for beacon advertising.
    beacon_init.beacon_addr.addr[0] += 2;

    app_beacon_init(&beacon_init);
    app_beacon_start();

    return NRF_SUCCESS;
}


/**@brief Function for updating advdata
 */
void eg_update_advdata(void) {
    uint32_t err_code;
    ble_gap_conn_sec_mode_t sec_mode;
    ble_advertising_init_t init;
    static ble_uuid_t adv_uuids[] = {{BLE_UUID_NUS_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN}};

    NRF_LOG_DEBUG("Updating advdata...");

    // Set device name
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    err_code = sd_ble_gap_device_name_set(&sec_mode, g_device_name, g_device_name_len);
    APP_ERROR_CHECK(err_code);

    // Set TX power
//    sd_ble_gap_adv_stop();	//make sure not advertising (disconnect?)
//    g_tx_power_level = RADIO_TXPOWER_TXPOWER_Neg40dBm;
//    err_code = sd_ble_gap_tx_power_set(g_tx_power_level);
//
//    // Set advertising data struct
//    memset(&init, 0, sizeof(init));
//    init.advdata.name_type               = BLE_ADVDATA_NO_NAME;
//    init.advdata.include_appearance      = false;
//    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
//    init.advdata.uuids_more_available.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
//    init.advdata.uuids_more_available.p_uuids = adv_uuids;
//    init.advdata.p_manuf_specific_data = &manuf_data;
//
//    // Set scan response data
//    init.srdata.name_type = BLE_ADVDATA_FULL_NAME;
//    init.srdata.p_tx_power_level = &g_tx_power_level;
//
//    // Set advertising parameters
//    init.config.ble_adv_fast_enabled  = true;
//    init.config.ble_adv_fast_interval = FAST_APP_ADV_INTERVAL;
//    init.config.ble_adv_fast_timeout  = FAST_APP_ADV_DURATION;	
//    init.config.ble_adv_slow_enabled  = true;
//    init.config.ble_adv_slow_interval = SLOW_APP_ADV_INTERVAL;
//    init.config.ble_adv_slow_timeout  = SLOW_APP_ADV_DURATION;
//    init.evt_handler = on_adv_evt;
//    err_code = ble_advertising_init(&m_advertising, &init);
//    APP_ERROR_CHECK(err_code);
    
    // Disable, re-initialize, and start advertising again
////    err_code = sd_ble_gap_adv_stop(m_advertising.adv_handle);
////    APP_ERROR_CHECK(err_code);
////    advertising_init();
////    advertising_start();

    // TODO: Update advertising data on-the-fly
    // https://devzone.nordicsemi.com/f/nordic-q-a/35884/updating-advertising-data-in-sdk15
}

/**@brief Function for the Power manager.
 */
static void log_init(void) {
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


static void power_management_init(void) {
    uint32_t err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void) {
    if (NRF_LOG_PROCESS() == false) {
        nrf_pwr_mgmt_run();
    }
}


/**@brief Initialize EllieGrid interfaces and components
 */
static void eg_init(void) {
    uint32_t err_code;
    
    // Get HW version
    nrf_gpio_cfg_input(HW_ADDR_0_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(HW_ADDR_1_PIN, NRF_GPIO_PIN_NOPULL);
    
    // TODO: comment out for v2_0 hardware
    // Put external flash into power down mode
    //[TO DO] pull this power down mode into the MEM driver
    nrf_gpio_cfg_output(MEM_CS_PIN);
    nrf_gpio_pin_set(MEM_CS_PIN);
    
    // Init nRF52 communication peripherals
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
    eg_twi_init();
    
    // Init battery charger
    bc_init();	// commenting out reduces from ~0.55mA to ~0.15mA?

    // Put FG into operation mode before communicating to other I2C
    err_code = fg_init();
    APP_ERROR_CHECK(err_code);

    // Init external peripherals
    mag_init();
    
    // CAPTEST
    captouch_init(MTCH_POWER_LOW);  //MTCH_POWER_NORMAL
    buttons_init();
    
    // Init DC2 and powered components
    dc2_init();
    eg_leds_init();

    // Init timers for current time
//    rtc_init();
//    rtc_start();
    
    // Init and fetch flash data storage (FDS)
    eg_fds_init();
    err_code = eg_fds_fetch_user_prefs();
    APP_ERROR_CHECK(err_code);

    // Update pillbox notify state
    g_notify_state = 0;
    g_notify_state |= (!nrfx_gpiote_in_is_set(CAP_OUTn_PIN)) ? STATUS_BTN_PRESSED_MASK : 0;
    g_notify_state |= (get_mag_sensor_state()) ? STATUS_LID_OPEN_MASK : 0;
    g_notify_state |= (get_bc_charging_state()) ? STATUS_CHARGING_MASK : 0;
    
    // Disable TWI to save power
    eg_twi_disable();
}


/**@brief Function for application main entry.
 */
int main(void) {
    uint32_t err_code;

    // Initialize nRF52 and BLE
    log_init();
    timers_init();
    power_management_init();
    
    // Init EllieGrid
    eg_init();
    
    // Init BLE
    ble_stack_init();

    // TODO: Enable DCDC, but switch to LDO during low current modes
//    err_code = nrf_drv_power_init(NULL);
//    APP_ERROR_CHECK(err_code);
    sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);

    if (BEACON_EN == true) {
      // Run beacon advertising example
      beacon_advertising_init();
      beacon_advertising_start();
    } else if (TIMESLOT_EN == true) {
      // Run beacon advertising using timeslot API
//      err_code = timeslot_init();
//      if (err_code != NRF_SUCCESS) {
//        NRF_LOG_ERROR("timeslot_init failed - %d\r\n", err_code);
//        return err_code;
//      }

      // Run connectable BLE advertising
      gap_params_init();
      gatt_init();
      services_init();
      advertising_init();
      conn_params_init();

      // Set TX power (supported: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +3dBm and +4dBm)
      err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_advertising.adv_handle, g_tx_power_level);
      APP_ERROR_CHECK(err_code);

      // Start execution
      battery_level_update();
      application_timers_start();
      advertising_start();
    }

    NRF_LOG_INFO("stranger-things-wall firmware v%s started!", FW_REVISION);

    // Enter main loop
    for (;;) {
        idle_state_handle();
    }
}

/**
 * @}
 */
