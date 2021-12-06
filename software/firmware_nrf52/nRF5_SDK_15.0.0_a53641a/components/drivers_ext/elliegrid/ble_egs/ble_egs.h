#ifndef BLE_EGS_H__
#define BLE_EGS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief   Macro for defining a ble_egs instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_EGS_DEF(_name)                                                                   \
ble_egs_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                          \
                     BLE_EGS_BLE_OBSERVER_PRIO,                                              \
                     ble_egs_on_ble_evt, &_name)

#define EGS_UUID_BASE        {0x51, 0xBB, 0x06, 0x63, 0x51, 0x0B, 0xF4, 0x9A, \
                              0x29, 0x4C, 0x40, 0xD4, 0x00, 0x00, 0x1E, 0xE1}
#define EGS_UUID_SERVICE     			0x0001
#define EGS_UUID_COMP_LED_CHAR                  0x0002
#define EGS_UUID_STATUS_CHAR			0x0003		//global variable state for PGn, MAGn, CAPn, other interrupts, haptic enable, 

#define COMMAND_LEDS                            0x1
#define COMMAND_SELECT_LEDS                     0x2
#define COMMAND_AUDIO                           0x3
#define COMMAND_AUDIO_NO_VOL_CHG                0x4
#define COMMAND_EASTER_EGG			0x5
#define COMMAND_ALARM_TRIGGER			0x6
#define COMMAND_ALARM_TAKEN			0x7
#define COMMAND_ALARM_SKIPPED			0x8
#define COMMAND_ALARM_MISSED			0x9
#define COMMAND_PILLBOX_STATE			0xA
#define COMMAND_ALARM_SILENCED			0xB
#define COMMAND_RICK				0x5249434B	//'RICK' <--> 0x4B434952
#define COMMAND_LEDS_RAINBOW			0x50000001
#define COMMAND_LEDS_RAINBOW_CYCLE		0x50000002
#define COMMAND_LEDS_THEATER_CHASE		0x50000003
#define COMMAND_LEDS_THEATER_CHASE_RAINBOW	0x50000004

// STATUS NOTIFICATION (byte) = 0b0000-0-CHARGING-LID-BTN
#define STATUS_BTN_PRESSED_MASK			0x01
#define STATUS_LID_OPEN_MASK			0x02
#define STATUS_CHARGING_MASK			0x04	

// Forward declaration of the ble_egs_t type.
typedef struct ble_egs_s ble_egs_t;

typedef void (*ble_egs_led_write_handler_t) (uint16_t conn_handle, ble_egs_t * p_egs, uint8_t new_state);
typedef void (*ble_egs_comp_led_write_handler_t)(uint16_t conn_handle, ble_egs_t *p_egs, uint32_t new_state);

/** @brief LED Button Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_egs_led_write_handler_t led_write_handler; /**< Event handler to be called when the LED Characteristic is written. */
    ble_egs_comp_led_write_handler_t comp_led_write_handler;
} ble_egs_init_t;

/**@brief LED Button Service structure. This structure contains various status information for the service. */
struct ble_egs_s
{
    uint16_t                    service_handle;      /**< Handle of LED Button Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    led_char_handles;    /**< Handles related to the LED Characteristic. */
    ble_gatts_char_handles_t    button_char_handles; /**< Handles related to the Button Characteristic. */
    ble_gatts_char_handles_t    comp_led_char_handles;
    uint8_t                     uuid_type;           /**< UUID type for the LED Button Service. */
    ble_egs_led_write_handler_t led_write_handler;   /**< Event handler to be called when the LED Characteristic is written. */
    ble_egs_comp_led_write_handler_t comp_led_write_handler;
};


/**@brief Function for initializing the LED Button Service.
 *
 * @param[out] p_egs      LED Button Service structure. This structure must be supplied by
 *                        the application. It is initialized by this function and will later
 *                        be used to identify this particular service instance.
 * @param[in] p_egs_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was initialized successfully. Otherwise, an error code is returned.
 */
uint32_t ble_egs_init(ble_egs_t * p_egs, const ble_egs_init_t * p_egs_init);


/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the LED Button Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  LED Button Service structure.
 */
void ble_egs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief Function for sending a button state notification.
 *
 ' @param[in] conn_handle   Handle of the peripheral connection to which the button state notification will be sent.
 * @param[in] p_egs         LED Button Service structure.
 * @param[in] button_state  New button state.
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_egs_on_button_change(uint16_t conn_handle, ble_egs_t * p_egs, uint8_t button_state);


#ifdef __cplusplus
}
#endif

#endif // BLE_EGS_H__

/** @} */
