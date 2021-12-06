#include "eg_leds.h"

APP_TIMER_DEF(m_led_select_timer_id);
APP_TIMER_DEF(m_battery_display_timer_id);
APP_TIMER_DEF(m_led_off_timer_id);
uint8_t l_select_led_comp;	//represents compartment to display LED animation
uint8_t l_select_led_pattern;	//represents LED animation pattern

void eg_leds_init(void) {
    ret_code_t err_code;
    
    // Init LED select timer
    err_code = app_timer_create(&m_led_select_timer_id, APP_TIMER_MODE_REPEATED, led_select_timer_handler);
    APP_ERROR_CHECK(err_code);
    
    // Init LED battery display timer
    err_code = app_timer_create(&m_battery_display_timer_id, APP_TIMER_MODE_SINGLE_SHOT, battery_display_timer_handler);
    APP_ERROR_CHECK(err_code);
    
    // Init LED off display time
    err_code = app_timer_create(&m_led_off_timer_id, APP_TIMER_MODE_SINGLE_SHOT, led_off_timer_handler);
    APP_ERROR_CHECK(err_code);
    
    l_select_led_comp = 0;
    l_select_led_pattern = DEFAULT_LED_SEL_PATTERN;
}

void eg_leds_off(void) {
    if (dc2_state() == DC2_ON) {
	// Stop LED timer
	eg_leds_stop_led_select_timer();
    
	// Turn off LEDs
	clear_leds();
	
	// Add delay to prevent DC2 from turning off before clear_leds finishes
	nrf_delay_us(150);
//	nrf_delay_us(100);  [causes lingering compartment 4 LEDs during animations 
//	nrf_delay_us(200);  [causes high power when skipping alarms ~6mA]
//	while (g_i2s_data_sent_flag == 0);  // this didn't work for some reason

	// Turn off DC2
	dc2_disable();
	
	// Stop and uninitialize the I2S interface to reduce current consumption
	nrf_drv_i2s_stop();
	nrf_drv_i2s_uninit();
    }
}

void clear_leds() {
    memset(m_i2s_led_buffer_tx, 0x88, I2S_LEDS_WORD_SIZE * 4);
    send_i2s_led_data(LED_DIN_PIN);
}

void set_compartment_leds(uint8_t compartment, uint8_t val, uint8_t r, uint8_t g, uint8_t b) {
    switch(compartment) {
	case 1 :
	    if (val == 1) {					
		set_led_pixel_RGB(1, r, g, b);	//set middle LED
	    } else if (val == 2) {					
		set_led_pixel_RGB(0, r, g, b);	//set two outside LEDs
		set_led_pixel_RGB(2, r, g, b);
	    } else if (val == 3) {
		set_led_pixel_RGB(0, r, g, b);	//set all LEDs
		set_led_pixel_RGB(1, r, g, b);
		set_led_pixel_RGB(2, r, g, b);
	    } else if (val == 4) {
		set_led_pixel_RGB(2, r, g, b);	//set right LED
	    } else if (val == 5) {
		set_led_pixel_RGB(1, r, g, b);	//set right 2 LEDs
		set_led_pixel_RGB(2, r, g, b);
	    } else if (val == 6) {
		set_led_pixel_RGB(0, r, g, b);	//set left LED
	    } else if (val == 7) {
		set_led_pixel_RGB(0, r, g, b);	//set left 2 LEDs
		set_led_pixel_RGB(1, r, g, b);
	    } else{
		set_led_pixel_RGB(0, 0, 0, 0);	//set all LEDs off
		set_led_pixel_RGB(1, 0, 0, 0);
		set_led_pixel_RGB(2, 0, 0, 0);
	    }
	    break;
	    
	case 2 :
	    if (val == 1) {					
		set_led_pixel_RGB(4, r, g, b);	//set middle LED
	    } else if (val == 2) {					
		set_led_pixel_RGB(3, r, g, b);	//set two outside LEDs
		set_led_pixel_RGB(5, r, g, b);
	    } else if (val == 3) {
		set_led_pixel_RGB(3, r, g, b);	//set all LEDs
		set_led_pixel_RGB(4, r, g, b);
		set_led_pixel_RGB(5, r, g, b);
	    } else if (val == 4) {
		set_led_pixel_RGB(5, r, g, b);	//set right LED
	    } else if (val == 5) {
		set_led_pixel_RGB(4, r, g, b);	//set right 2 LEDs
		set_led_pixel_RGB(5, r, g, b);
	    } else if (val == 6) {
		set_led_pixel_RGB(3, r, g, b);	//set left LED
	    } else if (val == 7) {
		set_led_pixel_RGB(3, r, g, b);	//set left 2 LEDs
		set_led_pixel_RGB(4, r, g, b);
	    } else{
		set_led_pixel_RGB(3, 0, 0, 0);	//set all LEDs off
		set_led_pixel_RGB(4, 0, 0, 0);
		set_led_pixel_RGB(5, 0, 0, 0);
	    }
	    break;
	    
	case 3 :
	    if (val == 1) {					
		set_led_pixel_RGB(7, r, g, b);	//set middle LED
	    } else if (val == 2) {					
		set_led_pixel_RGB(6, r, g, b);	//set two outside LEDs
		set_led_pixel_RGB(8, r, g, b);
	    } else if (val == 3) {
		set_led_pixel_RGB(6, r, g, b);	//set all LEDs
		set_led_pixel_RGB(7, r, g, b);
		set_led_pixel_RGB(8, r, g, b);
	    } else if (val == 4) {
		set_led_pixel_RGB(8, r, g, b);	//set right LED
	    } else if (val == 5) {
		set_led_pixel_RGB(7, r, g, b);	//set right 2 LEDs
		set_led_pixel_RGB(8, r, g, b);
	    } else if (val == 6) {
		set_led_pixel_RGB(6, r, g, b);	//set left LED
	    } else if (val == 7) {
		set_led_pixel_RGB(6, r, g, b);	//set left 2 LEDs
		set_led_pixel_RGB(7, r, g, b);
	    } else{
		set_led_pixel_RGB(6, 0, 0, 0);	//set all LEDs off
		set_led_pixel_RGB(7, 0, 0, 0);
		set_led_pixel_RGB(8, 0, 0, 0);
	    }
	    break;
	    
	case 4 :
	    if (val == 1) {					
		set_led_pixel_RGB(19, r, g, b);	//set middle LED
	    } else if (val == 2) {					
		set_led_pixel_RGB(18, r, g, b);	//set two outside LEDs
		set_led_pixel_RGB(20, r, g, b);
	    } else if (val == 3) {
		set_led_pixel_RGB(18, r, g, b);	//set all LEDs
		set_led_pixel_RGB(19, r, g, b);
		set_led_pixel_RGB(20, r, g, b);
	    } else if (val == 4) {
		set_led_pixel_RGB(18, r, g, b);	//set right LED
	    } else if (val == 5) {
		set_led_pixel_RGB(18, r, g, b);	//set right 2 LEDs
		set_led_pixel_RGB(19, r, g, b);
	    } else if (val == 6) {
		set_led_pixel_RGB(20, r, g, b);	//set left LED
	    } else if (val == 7) {
		set_led_pixel_RGB(19, r, g, b);	//set left 2 LEDs
		set_led_pixel_RGB(20, r, g, b);
	    } else{
		set_led_pixel_RGB(18, 0, 0, 0);	//set all LEDs off
		set_led_pixel_RGB(19, 0, 0, 0);
		set_led_pixel_RGB(20, 0, 0, 0);
	    }
	    break;
	    
	case 5 :
	    if (val == 1) {					
		set_led_pixel_RGB(16, r, g, b);	//set middle LED
	    } else if (val == 2) {					
		set_led_pixel_RGB(15, r, g, b);	//set two outside LEDs
		set_led_pixel_RGB(17, r, g, b);
	    } else if (val == 3) {
		set_led_pixel_RGB(15, r, g, b);	//set all LEDs
		set_led_pixel_RGB(16, r, g, b);
		set_led_pixel_RGB(17, r, g, b);
	    } else if (val == 4) {
		set_led_pixel_RGB(15, r, g, b);	//set right LED
	    } else if (val == 5) {
		set_led_pixel_RGB(15, r, g, b);	//set right 2 LEDs
		set_led_pixel_RGB(16, r, g, b);
	    } else if (val == 6) {
		set_led_pixel_RGB(17, r, g, b);	//set left LED
	    } else if (val == 7) {
		set_led_pixel_RGB(16, r, g, b);	//set left 2 LEDs
		set_led_pixel_RGB(17, r, g, b);
	    } else{
		set_led_pixel_RGB(15, 0, 0, 0);	//set all LEDs off
		set_led_pixel_RGB(16, 0, 0, 0);
		set_led_pixel_RGB(17, 0, 0, 0);
	    }
	    break;
	    
	case 6 :
	    if (val == 1) {					
		set_led_pixel_RGB(13, r, g, b);	//set middle LED
	    } else if (val == 2) {					
		set_led_pixel_RGB(12, r, g, b);	//set two outside LEDs
		set_led_pixel_RGB(14, r, g, b);
	    } else if (val == 3) {
		set_led_pixel_RGB(12, r, g, b);	//set all LEDs
		set_led_pixel_RGB(13, r, g, b);
		set_led_pixel_RGB(14, r, g, b);
	    } else if (val == 4) {
		set_led_pixel_RGB(12, r, g, b);	//set right LED
	    } else if (val == 5) {
		set_led_pixel_RGB(12, r, g, b);	//set right 2 LEDs
		set_led_pixel_RGB(13, r, g, b);
	    } else if (val == 6) {
		set_led_pixel_RGB(14, r, g, b);	//set left LED
	    } else if (val == 7) {
		set_led_pixel_RGB(13, r, g, b);	//set left 2 LEDs
		set_led_pixel_RGB(14, r, g, b);
	    } else{
		set_led_pixel_RGB(12, 0, 0, 0);	//set all LEDs off
		set_led_pixel_RGB(13, 0, 0, 0);
		set_led_pixel_RGB(14, 0, 0, 0);
	    }
	    break;
	    
	case 7 :
	    if (val == 1) {					
		set_led_pixel_RGB(10, r, g, b);	//set middle LED
	    } else if (val == 2) {					
		set_led_pixel_RGB(9, r, g, b);	//set two outside LEDs
		set_led_pixel_RGB(11, r, g, b);
	    } else if (val == 3) {
		set_led_pixel_RGB(9, r, g, b);	//set all LEDs
		set_led_pixel_RGB(10, r, g, b);
		set_led_pixel_RGB(11, r, g, b);
	    } else if (val == 4) {
		set_led_pixel_RGB(9, r, g, b);	//set right LED
	    } else if (val == 5) {
		set_led_pixel_RGB(9, r, g, b);	//set right 2 LEDs
		set_led_pixel_RGB(10, r, g, b);
	    } else if (val == 6) {
		set_led_pixel_RGB(11, r, g, b);	//set left LED
	    } else if (val == 7) {
		set_led_pixel_RGB(10, r, g, b);	//set left 2 LEDs
		set_led_pixel_RGB(11, r, g, b);
	    } else{
		set_led_pixel_RGB(9, 0, 0, 0);	//set all LEDs off
		set_led_pixel_RGB(10, 0, 0, 0);
		set_led_pixel_RGB(11, 0, 0, 0);
	    }
	    break;
	    
	default :
	    //Throw exception
	    NRF_LOG_ERROR("\nTrying to set an unknown LED compartment.\n");
    }
}
    

void drive_compartment_leds(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7, uint8_t r, uint8_t g, uint8_t b) {
//    dc2_enable();   // Enable 5V supply for LEDs
    sk6812_i2s_init_mem();
    set_compartment_leds(1, c1, r, g, b);
    set_compartment_leds(2, c2, r, g, b);
    set_compartment_leds(3, c3, r, g, b);
    set_compartment_leds(4, c4, r, g, b);
    set_compartment_leds(5, c5, r, g, b);
    set_compartment_leds(6, c6, r, g, b);
    set_compartment_leds(7, c7, r, g, b);
    set_i2s_led_data();
    send_i2s_led_data(LED_DIN_PIN);
}

void drive_app_compartment_leds(uint32_t leds_data, uint8_t r, uint8_t g, uint8_t b) {
    if (leds_data == 0) {
	clear_leds();
    } else {
	drive_compartment_leds((leds_data & 0x0000000f)>>0, (leds_data & 0x000000f0)>>4, (leds_data & 0x00000f00)>>8, (leds_data & 0x0000f000)>>12, (leds_data & 0x000f0000)>>16, (leds_data & 0x00f00000)>>20, (leds_data & 0x0f000000)>>24, r, g, b);    
    }
}

void blink_compartment_leds(uint8_t num_blink, uint32_t ms_delay, uint8_t r, uint8_t g, uint8_t b) {	
    for (uint8_t ii=0; ii < num_blink; ii++) {	
	drive_app_compartment_leds(0x03333333, r, g, b);
	nrf_delay_ms(ms_delay);
	clear_leds();
	nrf_delay_ms(ms_delay);
    }
}

void blink_offset_compartment_leds(uint8_t num_blink, uint32_t ms_delay) {
    uint8_t r1 = g_led_r;
    uint8_t g1 = g_led_g;
    uint8_t b1 = g_led_b;
    uint8_t r2 = g_led_r;
    uint8_t g2 = g_led_g;
    uint8_t b2 = g_led_b;
    for (uint8_t ii=0; ii < num_blink; ii++) {	
	drive_app_compartment_leds(0x01111111, r1, g1, b1);
	nrf_delay_ms(ms_delay);
	drive_app_compartment_leds(0x02222222, r2, g2, b2);
	nrf_delay_ms(ms_delay);
    }
    clear_leds();
}

void color_wipe(uint8_t r, uint8_t g, uint8_t b, uint8_t ms_delay) {
    sk6812_i2s_init_mem();
    for (uint8_t ii=0; ii < NUM_LEDS; ii++) {
	set_led_pixel_RGB(ii, r, g, b);
	set_i2s_led_data();
	send_i2s_led_data(LED_DIN_PIN);
	nrf_delay_ms(ms_delay);
    }
    for (uint8_t jj=0; jj < NUM_LEDS; jj++) {
	set_led_pixel_RGB(jj, 0, 0, 0);
	set_i2s_led_data();
	send_i2s_led_data(LED_DIN_PIN);
	nrf_delay_ms(ms_delay);
    }
}

void eg_set_single_led(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint16_t sdout_pin) {
    sk6812_i2s_init_mem();
    set_led_pixel_RGB(n, r, g, b);
    set_i2s_led_data();
    send_i2s_led_data(sdout_pin);
}


// Input a value 0 to 255 to get a color value
sk6812_led_t wheel(uint8_t wheel_pos) {
    sk6812_led_t color;
    wheel_pos = 255 - wheel_pos;
    
    if (wheel_pos < 85) {
	color.r = 255 - wheel_pos * 3;
	color.g = 0;
	color.b = wheel_pos * 3;
	return color;
    }
    
    if (wheel_pos < 170) {
	wheel_pos -= 85;
	color.r = 0;
	color.g = wheel_pos * 3;
	color.b = 255 - wheel_pos * 3;
	return color;	
    }
    
    wheel_pos -= 170;
    color.r = wheel_pos * 3;
    color.g = 255 - wheel_pos * 3;
    color.b = 0;
    return color;
}

void rainbow(uint16_t ms_delay) {
    uint16_t ii, jj;
    sk6812_led_t color;
    
    sk6812_i2s_init_mem();
    for (jj=0; jj < NUM_LEDS; jj++) {
	for (ii=0; ii < NUM_LEDS; ii++) {
	    color = wheel(((ii * 256 / NUM_LEDS) + jj) & 255);
	    set_led_pixel_RGB(ii, color.r, color.g, color.b);
	}
	set_i2s_led_data();
	send_i2s_led_data(LED_DIN_PIN);
	nrf_delay_ms(ms_delay);
    }
    clear_leds();
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbow_cycle(uint16_t ms_delay) {
    uint16_t ii, jj;
    sk6812_led_t color;
    
    sk6812_i2s_init_mem();
    for(jj=0; jj < 256*1; jj++) { // 1 cycle of all colors on wheel
	for(ii=0; ii < NUM_LEDS; ii++) {
	    color = wheel(((ii * 256 / NUM_LEDS) + jj) & 255);
	    set_led_pixel_RGB(ii, color.r, color.g, color.b);
	}
	set_i2s_led_data();
	send_i2s_led_data(LED_DIN_PIN);
	nrf_delay_ms(ms_delay);
    }
    clear_leds();
}

// Theater-style crawling lights.
void theater_chase(uint8_t r, uint8_t g, uint8_t b, uint16_t ms_delay) {
    sk6812_i2s_init_mem();
    for (int jj=0; jj < 10; jj++) {  //do 10 cycles of chasing
	for (int qq=0; qq < 3; qq++) {
	    for (uint16_t ii=0; ii < NUM_LEDS; ii=ii+3) {
		set_led_pixel_RGB(ii+qq, r, g, b);	//turn every third pixel on
	    }
	    set_i2s_led_data();
	    send_i2s_led_data(LED_DIN_PIN);

	    nrf_delay_ms(ms_delay);

	    for (uint16_t ii=0; ii < NUM_LEDS; ii=ii+3) {
		set_led_pixel_RGB(ii+qq, 0, 0, 0);	//turn every third pixel off
	    }
	}
    }
    clear_leds();
}

// Theater-style crawling lights with rainbow effect
void theater_chase_rainbow(uint16_t ms_delay) {
    sk6812_led_t color;
    sk6812_i2s_init_mem();
    for (int jj=0; jj < 256; jj++) {     // cycle all 256 colors in the wheel
	for (int qq=0; qq < 3; qq++) {
	    for (uint16_t ii=0; ii < NUM_LEDS; ii=ii+3) {
		color = wheel((ii+jj) % 255);
		set_led_pixel_RGB(ii+qq, color.r, color.g, color.b);   //turn every third pixel on
	    }
	    set_i2s_led_data();
	    send_i2s_led_data(LED_DIN_PIN);

	    nrf_delay_ms(ms_delay);

	    for (uint16_t ii=0; ii < NUM_LEDS; ii=ii+3) {
		set_led_pixel_RGB(ii+qq, 0, 0, 0);  //turn every third pixel off
	    }
	}
    }
    clear_leds();
}

// Blink all LEDs
void all_leds_blink(uint8_t r, uint8_t g, uint8_t b, uint16_t on_ms_delay, uint16_t off_ms_delay) {
    sk6812_i2s_init_mem();
    for (int ii=0; ii < NUM_LEDS; ii++) {
        set_led_pixel_RGB(ii, r, g, b);
    }
    set_i2s_led_data();
    send_i2s_led_data(LED_DIN_PIN);
    nrf_delay_ms(on_ms_delay);
    clear_leds();
    nrf_delay_ms(off_ms_delay);
}

void eg_leds_animate_select(uint8_t select_led_comp, uint8_t select_led_pattern) {
    l_select_led_comp = select_led_comp;
    l_select_led_pattern = select_led_pattern;
    
    // Stop existing LED animation timer?
    eg_leds_stop_led_select_timer();

    eg_leds_start_led_select_timer(DEFAULT_LED_SELECT_DURATION_MS);
}

void eg_leds_start_led_select_timer(uint32_t timer_ms) {
    APP_ERROR_CHECK(app_timer_start(m_led_select_timer_id, APP_TIMER_TICKS(timer_ms), NULL));
}

void eg_leds_stop_led_select_timer(void) {
    APP_ERROR_CHECK(app_timer_stop(m_led_select_timer_id));
}

void led_select_timer_handler(void * p_context) {
    static uint8_t led_select_count;
    uint8_t blink_state;
    
    // Display LED pattern
    switch (l_select_led_pattern) {
	case LED_SEL_PATTERN_1ON1OFF:
	    if (led_select_count % 2) {
		drive_app_compartment_leds(0x1<<((l_select_led_comp-1)*4), g_led_r, g_led_g, g_led_b);
	    } else {
		clear_leds();
	    }
	    break;

	case LED_SEL_PATTERN_3ON3OFF:
	    if (led_select_count % 2) {
		drive_app_compartment_leds(0x3<<((l_select_led_comp-1)*4), g_led_r, g_led_g, g_led_b);
	    } else {
		clear_leds();
	    }
	    break;

	case LED_SEL_PATTERN_123OFF:
	    blink_state = led_select_count % 4;
	    if (blink_state == 1) {
		drive_app_compartment_leds(0x6<<((l_select_led_comp-1)*4), g_led_r, g_led_g, g_led_b);
	    } else if (blink_state == 2) {
		drive_app_compartment_leds(0x7<<((l_select_led_comp-1)*4), g_led_r, g_led_g, g_led_b);
	    } else if (blink_state == 3) {
		drive_app_compartment_leds(0x3<<((l_select_led_comp-1)*4), g_led_r, g_led_g, g_led_b);
	    }
	    else {
		clear_leds();
	    }
	    break;
		
	case LED_SEL_PATTERN_ALL1ONOFF:
            if (led_select_count % 2) {
                drive_app_compartment_leds(0x01111111, g_led_r, g_led_g, g_led_b);
            } else {
                clear_leds();
            }
            break;
		
	case LED_SEL_PATTERN_ALL3ONOFF:
	    if (led_select_count % 2) {
		drive_app_compartment_leds(0x03333333, g_led_r, g_led_g, g_led_b);
	    } else {
		clear_leds();
	    }
	    break;

	case LED_SEL_PATTERN_ALL123:
	    blink_state = led_select_count % 4;
	    if (blink_state == 1) {
		drive_app_compartment_leds(0x06666666, g_led_r, g_led_g, g_led_b);
	    } else if (blink_state == 2) {
		drive_app_compartment_leds(0x07777777, g_led_r, g_led_g, g_led_b);
	    } else if (blink_state == 3) {
		drive_app_compartment_leds(0x03333333, g_led_r, g_led_g, g_led_b);
	    }
	    else {
		clear_leds();
	    }
	    break;
		
	case LED_SEL_PATTERN_SNAKE:
	    eg_set_single_led((led_select_count % NUM_LEDS), g_led_r, g_led_g, g_led_b, LED_DIN_PIN);
	    break;

        case LED_SEL_PATTERN_ALLONOFF:
            all_leds_blink(g_led_r, g_led_g, g_led_b, 500, 100);
            break;

	default:
	    // no implementation
	    break;
    }
    
    led_select_count++;
}

void display_led_battery_level(uint8_t led_bat_lvl) {
    if ((led_bat_lvl >= 0) && (led_bat_lvl <= 10)) {
	// Display all 10 LEDs first in RED
	sk6812_i2s_init_mem();
	uint8_t ii;
	uint8_t jj;
	for (ii=(BAT_LEVEL_LED_1-1); ii>=(BAT_LEVEL_LED_10-1); ii--) {
	    set_led_pixel_RGB(ii, RESET_LED_R, RESET_LED_G, RESET_LED_B);
	}
	set_i2s_led_data();
	send_i2s_led_data(LED_DIN_PIN);
	
	// TODO: use app_timers instead of nrf_delay_ms?
//	eg_leds_start_battery_display_timer(BATTERY_DISPLAY_TIME_MS);	
	// Pause before animating battery level
	nrf_delay_ms(300);
    
	// Display increasing battery level animation in GREEN
	if (led_bat_lvl > 0) {
	    for (ii=(BAT_LEVEL_LED_1-1), jj=1; ii>=(BAT_LEVEL_LED_10-1); ii--, jj++) {
		if (jj <= led_bat_lvl) {
		    set_led_pixel_RGB(ii, BATTERY_LED_R, BATTERY_LED_G, BATTERY_LED_B);
		    set_i2s_led_data();
		    send_i2s_led_data(LED_DIN_PIN);
		    nrf_delay_ms(100);
		}
	    }
	    // Show battery level for 3 seconds
//	    set_i2s_led_data();
//	    send_i2s_led_data(LED_DIN_PIN);
	    eg_leds_start_battery_display_timer(BATTERY_DISPLAY_TIME_MS);	
	}
    }
}

void eg_leds_start_battery_display_timer(uint32_t timer_ms) {
    APP_ERROR_CHECK(app_timer_start(m_battery_display_timer_id, APP_TIMER_TICKS(timer_ms), NULL));
}

void eg_leds_stop_battery_display_timer(void) {
    APP_ERROR_CHECK(app_timer_stop(m_battery_display_timer_id));
}

void battery_display_timer_handler(void * p_context) {
    eg_leds_off();
}

void eg_leds_animate_battery_level(void) {
    // Calculate number of LEDs to illuminate
    uint8_t led_bat_level;
    if (g_last_rsoc <= 15) {
	led_bat_level = 1;
    } else if (g_last_rsoc <= 25) {
	led_bat_level = 2;
    } else if (g_last_rsoc <= 35) {
	led_bat_level = 3;
    } else if (g_last_rsoc <= 45) {
	led_bat_level = 4;
    } else if (g_last_rsoc <= 55) {
	led_bat_level = 5;
    } else if (g_last_rsoc <= 65) {
	led_bat_level = 6;
    } else if (g_last_rsoc <= 75) {
	led_bat_level = 7;
    } else if (g_last_rsoc <= 85) {
	led_bat_level = 8;
    } else if (g_last_rsoc <= 95) {
	led_bat_level = 9;
    } else {
	led_bat_level = 10;
    }

    // Display estimated battery life
    // Illuminate bottom center 10 LEDs for 0-100% range (90-100% = 9 LEDs, 10-20% = 1 LED)
    // LED mapping: 1-#20, 2-#19, ... 9-#12, 10-#11
    display_led_battery_level(led_bat_level);
}

void eg_leds_start_led_off_timer(uint32_t timer_ms) {
    APP_ERROR_CHECK(app_timer_start(m_led_off_timer_id, APP_TIMER_TICKS(timer_ms), NULL));
}

void eg_leds_stop_led_off_timer(void) {
    APP_ERROR_CHECK(app_timer_stop(m_led_off_timer_id));
}

void led_off_timer_handler(void * p_context) {
//    // Turn off LEDs during heart beat (while alarm is triggered)
//    if (g_status == DATA_INVALID) {
//        clear_leds();
//    }
    clear_leds();
}

bool abc_validate(char c) {
    if (c >= 'A' && c <= 'Z') {
        return true;
    } else if (c >= 'a' && c <= 'z') {
        return true;
    } else if (c == ' ') {
        return true;
    } else {
	return false;
    }
}

void abc_display_char(char c) {
    // If character is a space, turn off LEDs for g_space_duration
    if (c == ' ') {
        clear_leds();
        nrf_delay_ms(g_space_duration_ms);
    }
    
    // If character is ABC, display character LED for g_char_duration and inter-character delay with LEDs off
    else {
        uint8_t led_idx;
        if (c >= 'A' && c <= 'Z') {
            led_idx = c - 'A';
            eg_set_single_led(led_idx, g_led_r, g_led_g, g_led_b, LED_DIN_PIN);
            nrf_delay_ms(g_char_duration_ms);
            clear_leds();
            nrf_delay_ms(g_inter_char_duration_ms);
        } else if (c >= 'a' && c <= 'z') {
            led_idx = c - 'a';
            eg_set_single_led(led_idx, g_led_r, g_led_g, g_led_b, LED_DIN_PIN);
            nrf_delay_ms(g_char_duration_ms);
            clear_leds();
            nrf_delay_ms(g_inter_char_duration_ms);
        }
    }
}

void abc_display_string(char str[]) {
    char c;
    uint16_t len = strlen(str);

    for (uint16_t ii=0; ii < len; ii++) {
        c = str[ii];

        // If character is a space, turn off LEDs for g_space_duration
        if (c == ' ') {
            clear_leds();
            nrf_delay_ms(g_space_duration_ms);
        }
    
        // If character is ABC, display character LED for g_char_duration and inter-character delay with LEDs off
        else {
            uint8_t led_idx;
            if (c >= 'A' && c <= 'Z') {
                led_idx = c - 'A';
                eg_set_single_led(led_idx, g_led_r, g_led_g, g_led_b, LED_DIN_PIN);
                nrf_delay_ms(g_char_duration_ms);
                clear_leds();
                nrf_delay_ms(g_inter_char_duration_ms);
            } else if (c >= 'a' && c <= 'z') {
                led_idx = c - 'a';
                eg_set_single_led(led_idx, g_led_r, g_led_g, g_led_b, LED_DIN_PIN);
                nrf_delay_ms(g_char_duration_ms);
                clear_leds();
                nrf_delay_ms(g_inter_char_duration_ms);
            }
        }
    }
}

void stranger_things_easter_egg_0(void) {
    abc_display_string("right here");
    eg_leds_off();
}

void stranger_things_easter_egg_1(void) {
    abc_display_string("run ");
    all_leds_blink(g_led_r, g_led_g, g_led_b, 100, 100);
    all_leds_blink(g_led_r, g_led_g, g_led_b, 100, 100);
    all_leds_blink(g_led_r, g_led_g, g_led_b, 100, 100);
    all_leds_blink(g_led_r, g_led_g, g_led_b, 100, 100);
    all_leds_blink(g_led_r, g_led_g, g_led_b, 100, 100);
    all_leds_blink(g_led_r, g_led_g, g_led_b, 100, 100);
    all_leds_blink(g_led_r, g_led_g, g_led_b, 100, 100);
    all_leds_blink(g_led_r, g_led_g, g_led_b, 100, 100);
    theater_chase_rainbow(25);
    eg_leds_off();
}

void regina_easter_egg(void) {
    abc_display_string("send me where love is needed");
    all_leds_blink(g_led_r, g_led_g, g_led_b, 3000, 100);
    eg_leds_off();
}
