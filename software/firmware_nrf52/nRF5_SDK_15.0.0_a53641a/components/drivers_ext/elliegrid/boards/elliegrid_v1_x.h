/* Board specific definitions for the EllieGrid v1_x (v0.4, v1.1, v1.2) prototype */

#ifndef ELLIEGRID_V1_X_H
#define ELLIEGRID_V1_X_H

/* GPIO PIN ASSIGNMENTS */
// Unconnected IOs: 9, 10, 11, 18, 25, 26, 27, 28(AIN4)
// System
#define XTAL1_PIN			0
#define XTAL2_PIN			1
#define HW_ADDR_0_PIN			25  //3
#define HW_ADDR_1_PIN			26  //4
#define I2C_SDA_PIN			16
#define I2C_SCL_PIN			17
#define RESETn_PIN			21  // 18 on nRF52840 and BMD-340

// Power
#define FG_INTn_PIN			22
#define DC2_EN_PIN			23
#define BC_CHGn_PIN			29
#define BC_PGn_PIN			30
#define BAT_MON_EN_PIN			24
#define BAT_VOLT_PIN			31  // AIN7

// Sensors
#define MAG_OUTn_PIN			13
#define CAP_MTPM_PIN			14
#define CAP_OUTn_PIN			15
#define ACC_INT1_PIN			19
#define ACC_INT2_PIN			20
#define CAP_AIN_PIN			3
#define CAP_CHG_PIN			14
#define CAP_OUT_PIN			15
#define CAP_EN_PIN			14
#define CAP_AT42QT1010			false	//true

// Peripherals
#define LED_DIN_PIN			2
#define LED_LRCK_PIN			11
#define LED_SCK_PIN			18  // 21 on nRF52840 and BMD-340
#define CHG_LED_DIN_PIN			5
#define AMP_IN_P_PIN			7
#define AMP_IN_N_PIN			8
#define HAP_EN_PIN			12
#define MIC_DATA_PIN			6
#define MIC_CLK_PIN			9
#define MIC_EN_PIN			10

// DEPRECATED from v2_0 hardware
#define MEM_SO_PIN			3
#define MEM_CS_PIN			4
#define MEM_SCK_PIN			5
#define MEM_SI_PIN			6

// Low frequency clock source to be used by the SoftDevice
#define EG_CLOCK_LFCLKSRC	{.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}

/* CONSTANTS */
#define NUM_COMPARTMENTS		7
#define NUM_LEDS_PER_COMP		3
#define NUM_LEDS			26
#define MAX_BLE_PACKET_LENGTH		20
#define DISPLAY_ERROR_LED		0   // top left LED
#define DISPLAY_HB_LED			4   // top center LED
#define DISPLAY_CHARGE_LED		9   // bottom right LED (closest to USB connector)
#define HW_REVISION_V1_2		"1.2.0"
#define HW_REVISION_V2_0		"2.0.0"
#define HW_ADDR_V1_2			0x3 // floating inputs on P0.26, P0.25
#define HW_ADDR_V2_0			0x0 // grounded inputs on P0.26, P0.25

#define BEACON_EN                       false
#define TIMESLOT_EN                     true

#define TX_POWER_LOW                    RADIO_TXPOWER_TXPOWER_Neg20dBm        //RADIO_TXPOWER_TXPOWER_Neg8dBm
#define TX_POWER_BALANCED               RADIO_TXPOWER_TXPOWER_0dBm
#define TX_POWER_HIGH                   RADIO_TXPOWER_TXPOWER_Pos4dBm
#define DEFAULT_TX_POWER                TX_POWER_BALANCED   //TX_POWER_HIGH, TX_POWER_LOW

// GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS (7): 
// FG_INTn_PIN, BC_CHGn_PIN, BC_PGn_PIN, MAG_OUTn_PIN, CAP_OUTn_PIN, ACC_INT1_PIN, ACC_INT2_PIN
// APP_TIMER_CONFIG_OP_QUEUE_SIZE (10/20):
// eg_rtc, battery level check (R), app_button (SS), LED select comp (R), heartbeat (R), ping (R), missed (SS), display (SS), plug (SS), BLE conn params, BSP button long press, LED, alert [not used]

#endif // ELLIEGRID_V1_X_H
