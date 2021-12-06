// BLE CONSTANTS
export const SCAN_MODE_OPPORTUNISTIC = -1;
export const SCAN_MODE_LOW_POWER = 0;
export const SCAN_MODE_BALANCED = 1;
export const SCAN_MODE_LOW_LATENCY = 2;

// ELLIEGRID BLE PROFILE CONSTANTS
export const GAP_SERVICE_UUID = '1800';
export const DEVICE_NAME_CHAR = '2A00';
export const APPEARANCE_CHAR = '2A01';
export const PPCP_CHAR = '2A04';
export const CAR_CHAR = '2AA6';
export const GATT_SERVICE_UUID = '1801';
export const SERVICE_CHANGED_CHAR = '2A05';
export const DEVICE_INFO_SERVICE_UUID = '180A';
export const MANUF_NAME_CHAR = '2A29';
export const MODEL_NUM_CHAR = '2A24';
export const SERIAL_NUM_CHAR = '2A25';
export const HW_REV_CHAR = '2A27';
export const FW_REV_CHAR = '2A26';
export const SW_REV_CHAR = '2A28';
export const BATTERY_SERVICE_UUID = '180F';
export const BATTERY_LEVEL_CHAR_UUID = '2A19';
export const NOTIFY_BATTERY_SERVICE_UUID = '0000180F-0000-1000-8000-00805F9B34FB';
export const NOTIFY_BATTERY_LEVEL_CHAR_UUID = '00002A19-0000-1000-8000-00805F9B34FB';
export const EG_UART_SERVICE_UUID = 'EE930001-D90F-41CC-A980-9BFF12E722F4';
export const EG_UART_RX_CHAR_UUID = 'EE930002-D90F-41CC-A980-9BFF12E722F4';
export const EG_UART_TX_CHAR_UUID = 'EE930003-D90F-41CC-A980-9BFF12E722F4';
export const EG_COMMAND_SERVICE_UUID = 'E11E0001-D440-4C29-9AF4-0B516306BB51';
export const EG_COMMAND_CHAR_UUID = 'E11E0002-D440-4C29-9AF4-0B516306BB51';
export const EG_COMMAND_STATUS_CHAR_UUID = 'E11E0003-D440-4C29-9AF4-0B516306BB51';
export const NRF_DFU_SERVICE_UUID = '8EC9FE59-F315-4F60-9FB8-838830DAEA50';
export const NRF_DFU_BUTTONLESS_CHAR_UUID = '8EC9FE03-F315-4F60-9FB8-838830DAEA50';
export const NRF_DFU_CONTROL_POINT_CHAR_UUID = '8EC9FE01-F315-4F60-9FB8-838830DAEA50';
export const NRF_DFU_PACKET_CHAR_UUID = '8EC9FE02-F315-4F60-9FB8-838830DAEA50';

// ELLIEGRID BLE CONSTANTS
export const SCAN_SECONDS = 3;
export const TIMEOUT_MS_DISCONNECT = 10000;
export const TIMEOUT_MS_CONNECT = 10000;
export const TIMEOUT_MS_INITIALIZE = 10000;
export const KEEP_PENDING_TASK = true;
export const STATUS_BUTTON_PRESSED_MASK = 0x01;
export const STATUS_LID_OPEN_MASK = 0x02;
export const STATUS_CHARGING_MASK = 0x04;

// DATA HEADERS
export const HEADER_MESSAGE = [0x53, 0x54];
export const HEADER_USER_PREFS = [0x75, 0x70];
export const HEADER_SYSTEM_SETTING = [0xE8];
export const HEADER_LP_MODE = [0xE8, 0x01];
export const HEADER_SOFT_RESET = [0xE8, 0x02];
export const HEADER_DISCONNECT = [0xE8, 0x08];
export const HEADER_FACTORY_RESET = [0xDE, 0xAD];
export const HEADER_DEBUG = [0x64, 0x62];
export const HEADER_DEVICE_NAME = [0x64, 0x6E];
export const HEADER_SERIAL_NUM = [0x73, 0x6E];
export const HEADER_ALARM = [0xE7, 0x01];
export const HEADER_SMART_ALARM = [0xEA, 0x1A];
export const HEADER_ACK = [0xE7, 0x02];
export const HEADER_COMPL = [0xE7, 0x04];
export const HEADER_CURRENT_TIME = [0xE7, 0x08];
export const HEADER_ALARM_CHECKSUM = [0xE7, 0x10];
export const HEADER_ALARM_REQUEST = [0xE7, 0x11];

// USER PREFERENCES
//[HD0, HD1, KEY, GET/SET, value]
export const KEY_USER_PREFS_GET = 0;
export const KEY_USER_PREFS_SET = 1;
export const KEY_LED_R = 1;
export const KEY_LED_G = 2;
export const KEY_LED_B = 3;
export const KEY_LED_PATTERN = 4;
export const KEY_TX_POWER = 5;
export const KEY_CHAR_DURATION_MS = 6;              //uint32_t SET, uint16_t GET
export const KEY_INTER_CHAR_DURATION_MS = 7;        //uint32_t SET, uint16_t GET
export const KEY_SPACE_DURATION_MS = 8;             //uint32_t SET, uint16_t GET
export const DEFAULT_AUTOCONNECT_EN = true;

// LED CONSTANTS
export const COMPARTMENT1 = 0x01;
export const COMPARTMENT2 = 0x02;
export const COMPARTMENT3 = 0x03;
export const COMPARTMENT4 = 0x04;
export const COMPARTMENT5 = 0x05;
export const COMPARTMENT6 = 0x06;
export const COMPARTMENT7 = 0x07;
export const ALL_COMPARTMENTS = 0xFF;
export const LEDS_OFF_DATA = [0x00, 0x00, 0x00, 0x10];
export const ANIMATE_LEDS_STOP_DATA = [0x00, 0x00, 0x00, 0x20];
export const ANIMATE_LEDS_PATTERN_1ON1OFF = 1;
export const ANIMATE_LEDS_PATTERN_3ON3OFF = 2;
export const ANIMATE_LEDS_PATTERN_123OFF = 3;
export const ANIMATE_LEDS_PATTERN_ALL1ONOFF = 4;
export const ANIMATE_LEDS_PATTERN_ALL3ONOFF = 5;
export const ANIMATE_LEDS_PATTERN_ALL123 = 6;
export const ANIMATE_LEDS_PATTERN_SNAKE = 7;
export const BRIGHTNESS_LEVEL_OFF = 0;
export const BRIGHTNESS_LEVEL_LOW = 5;
export const BRIGHTNESS_LEVEL_MED_LOW = 50;
export const BRIGHTNESS_LEVEL_MED = 100;
export const BRIGHTNESS_LEVEL_MED_HIGH = 175;
export const BRIGHTNESS_LEVEL_HIGH = 255;
export const DEFAULT_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL_MED;
export const DEFAULT_BRIGHTNESS_SLIDER_LEVEL = 3;
export const BRIGHTNESS_LEVELS = [
    BRIGHTNESS_LEVEL_OFF,
    BRIGHTNESS_LEVEL_LOW,
    BRIGHTNESS_LEVEL_MED_LOW,
    BRIGHTNESS_LEVEL_MED,
    BRIGHTNESS_LEVEL_MED_HIGH,
    BRIGHTNESS_LEVEL_HIGH,
];

// AUDIO CONSTANTS
export const VOLUME_LEVEL_OFF = 0;
export const VOLUME_LEVEL_LOW = 1;
export const VOLUME_LEVEL_MED_LOW = 10;
export const VOLUME_LEVEL_MED = 15;
export const VOLUME_LEVEL_MED_HIGH = 20;
export const VOLUME_LEVEL_HIGH = 25;
export const VOLUME_LEVEL_HIGHER1 = 35;
export const VOLUME_LEVEL_HIGHER2 = 50;
export const VOLUME_LEVEL_HIGHER3 = 65;
export const VOLUME_LEVEL_MAX = 80; //100;
export const DEFAULT_VOLUME_LEVEL = VOLUME_LEVEL_MED;
export const DEFAULT_VOLUME_SLIDER_LEVEL = 3;
export const VOLUME_LEVELS = [
    VOLUME_LEVEL_OFF,
    VOLUME_LEVEL_LOW,
    VOLUME_LEVEL_MED_LOW,
    VOLUME_LEVEL_MED,
    VOLUME_LEVEL_MED_HIGH,
    VOLUME_LEVEL_HIGH,
    VOLUME_LEVEL_HIGHER1,
    VOLUME_LEVEL_HIGHER2,
    VOLUME_LEVEL_HIGHER3,
    VOLUME_LEVEL_MAX,
];
export const COMMAND_LEDS = 0x1;
export const COMMAND_SELECT_LEDS = 0x2;
export const COMMAND_AUDIO = 0x3;
export const COMMAND_AUDIO_NO_VOL_CHG = 0x4;
export const COMMAND_EASTER_EGG = 0x5;
export const COMMAND_ALARM_TRIGGER = 0x6;
export const COMMAND_ALARM_TAKEN = 0x7;
export const COMMAND_ALARM_SKIPPED = 0x8;
export const COMMAND_ALARM_MISSED = 0x9;
export const COMMAND_PILLBOX_STATE = 0xA;
export const COMMAND_RICK = [0x4B, 0x43, 0x49, 0x52];   //'RICK' reversed
export const COMMAND_LEDS_RAINBOW = [0x01, 0x00, 0x00, 0x50];
export const COMMAND_LEDS_RAINBOW_CYCLE = [0x02, 0x00, 0x00, 0x50];
export const COMMAND_LEDS_THEATER_CHASE = [0x03, 0x00, 0x00, 0x50];
export const COMMAND_LEDS_THEATER_CHASE_RAINBOW = [0x04, 0x00, 0x00, 0x50];
export const SOUND_SINGLE_BEEP = 0;
export const SOUND_SIMPLE_BEEP = 1;
export const SOUND_SIMPLE_KNOCK = 2;
export const SOUND_MARIO_ONE_UP = 3;
export const SOUND_MARIO_PAUSE = 4;
export const SOUND_MARIO_DEATH = 5;
export const SOUND_MARIO_TIME_WARNING = 6;
export const SOUND_MARIO_POWER_UP = 7;
export const SOUND_MARIO_COIN = 8;
export const SOUND_MARIO_FLAGPOLE_FANFARE = 9;
export const SOUND_MARIO_GAME_OVER = 10;
export const SOUND_LULLABY = 11;
export const SOUND_NOTIFICATION = 12;
export const SOUND_NOTIFICATION2 = 13;
export const SOUND_DUTY_CYCLE_TEST = 14;
export const SOUND_CHROMATIC_TEST = 15;
export const SOUND_NULL = 16;   // play nothing
export const SOUND_POKEMON_HEAL = 17;
export const SOUND_SONIC_HEAL = 18;
export const SOUND_ZELDA_SECRET = 19;
export const SOUND_ZELDA_ALERT = 20;
export const SOUND_ZELDA_KOROK = 21;
export const SOUND_POWER_RANGERS = 22;
export const SOUND_KIMPOSSIBLE = 23;
export const SOUND_PRICE_RIGHT = 24;
export const SOUND_ARPEGGIOS = 25;
export const SOUND_ARPEGGIOS2 = 26;
export const SOUND_ARPEGGIOS3 = 27;
export const SOUND_MAJOR_SCALE = 28;
export const SOUND_TEST_PWM_TONE = 0xFB;
export const SOUND_RICK = 0xFC;
export const SOUND_PHOENIX = 0xFD;
export const SOUND_GLASS_PING = 0xFE;
export const SOUND_JINGLE_SYNTH = 0xFF;

// FIRMWARE CONSTANTS
export const FW_URL_V2_0_2_APP = 'https://firebasestorage.googleapis.com/v0/b/elliegrid-app.appspot.com/o/firmware_updates%2Fv2_0_2_eg_app_dfu.zip?alt=media&token=1d9f8eb9-0476-4efe-bc7d-499fbbddfaa7';
export const FW_URL_V2_0_2_APP_SD_BL = 'https://firebasestorage.googleapis.com/v0/b/elliegrid-app.appspot.com/o/firmware_updates%2Fv2_0_2_eg_app_sd_bl_dfu.zip?alt=media&token=10fce9dd-cc12-48cd-a3f9-b78a944cfc8d';
export const FW_URL_V2_0_3_APP = 'https://firebasestorage.googleapis.com/v0/b/elliegrid-app.appspot.com/o/firmware_updates%2Fv2_0_3_eg_app_dfu.zip?alt=media&token=7d8999c9-a10d-4747-8415-9d5be8e17777';
export const FW_URL_V2_0_3_APP_SD_BL = 'https://firebasestorage.googleapis.com/v0/b/elliegrid-app.appspot.com/o/firmware_updates%2Fv2_0_3_eg_app_sd_bl_dfu.zip?alt=media&token=1ae350ff-b89a-469a-8996-aae2ccb8fed0';
export const FW_URL_V2_0_4_APP = 'https://firebasestorage.googleapis.com/v0/b/elliegrid-app.appspot.com/o/firmware_updates%2Fv2_0_4_eg_app_dfu.zip?alt=media&token=992e8e29-0dc5-4f60-b631-950e16c2763d';
export const FW_URL_V2_0_4_APP_SD_BL = 'https://firebasestorage.googleapis.com/v0/b/elliegrid-app.appspot.com/o/firmware_updates%2Fv2_0_4_eg_app_sd_bl_dfu.zip?alt=media&token=7151b934-f45e-4a79-9de8-be88a943e4a2';
export const FW_REVISION_LATEST = '2.0.4';
export const FW_REVISION_LATEST_URL_APP = FW_URL_V2_0_4_APP;
export const FW_REVISION_LATEST_URL_APP_SD_BL = FW_URL_V2_0_4_APP_SD_BL;

// POWER CONSTANTS
export const ALERT_LOW_BATTERY_LEVEL = 20;

// EASTER EGGS
export const EASTER_EGG_RIGHTHERE = 0x30;       // '0'
export const EASTER_EGG_RUN = 0x31;             // '1'
export const EASTER_EGG_REGINA = 0x32;          // '2'
export const EASTER_EGG_RAINBOW = 0x35;         // '5'
export const EASTER_EGG_THEATER_CHASE = 0x36;   // '6'