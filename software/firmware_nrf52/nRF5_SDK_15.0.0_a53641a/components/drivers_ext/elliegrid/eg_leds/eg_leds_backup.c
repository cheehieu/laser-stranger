#include "eg_leds.h"

// TODO: Port driver to use official NeoPixel and NRF52 library

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
    
    // Init LED off display timer
    err_code = app_timer_create(&m_led_off_timer_id, APP_TIMER_MODE_SINGLE_SHOT, led_off_timer_handler);
    APP_ERROR_CHECK(err_code);
	
    l_select_led_comp = 0;
    l_select_led_pattern = DEFAULT_ALARM_LED_SEL_PATTERN;
}

void eg_leds_off(void) {
    if (dc2_state() == DC2_ON) {
	// Stop LED timer
	eg_leds_stop_led_select_timer();
    
	// Turn off LEDs
	driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
    
	// Turn off DC2
	dc2_disable();
    }
}

void setCompartmentLEDs_NEO(uint8_t compartment, uint8_t val, uint8_t r, uint8_t g, uint8_t b) {
    switch(compartment) {
	case 1 :
	    if (val == 1) {					
		WS2812B_setPixelColorRGB(1, r, g, b, 1);	//set middle LED
	    } else if (val == 2) {					
		WS2812B_setPixelColorRGB(0, r, g, b, 1);	//set two outside LEDs
		WS2812B_setPixelColorRGB(2, r, g, b, 1);
	    } else if (val == 3) {
		WS2812B_setPixelColorRGB(0, r, g, b, 1);	//set all LEDs
		WS2812B_setPixelColorRGB(1, r, g, b, 1);
		WS2812B_setPixelColorRGB(2, r, g, b, 1);
	    } else if (val == 4) {
		WS2812B_setPixelColorRGB(2, r, g, b, 1);	//set right LED
	    } else if (val == 5) {
		WS2812B_setPixelColorRGB(1, r, g, b, 1);	//set right 2 LEDs
		WS2812B_setPixelColorRGB(2, r, g, b, 1);
	    } else if (val == 6) {
		WS2812B_setPixelColorRGB(0, r, g, b, 1);	//set left LED
	    } else if (val == 7) {
		WS2812B_setPixelColorRGB(0, r, g, b, 1);	//set left 2 LEDs
		WS2812B_setPixelColorRGB(1, r, g, b, 1);
	    } else{
		WS2812B_setPixelColorRGB(0, 0, 0, 0, 1);	//set all LEDs off
		WS2812B_setPixelColorRGB(1, 0, 0, 0, 1);
		WS2812B_setPixelColorRGB(2, 0, 0, 0, 1);
	    }
	    break;
	case 2 :
	    if (val == 1) {					
		WS2812B_setPixelColorRGB(4, r, g, b, 1);	//set middle LED
	    } else if (val == 2) {					
		WS2812B_setPixelColorRGB(3, r, g, b, 1);	//set two outside LEDs
		WS2812B_setPixelColorRGB(5, r, g, b, 1);
	    } else if (val == 3) {
		WS2812B_setPixelColorRGB(3, r, g, b, 1);	//set all LEDs
		WS2812B_setPixelColorRGB(4, r, g, b, 1);
		WS2812B_setPixelColorRGB(5, r, g, b, 1);
	    } else if (val == 4) {
		WS2812B_setPixelColorRGB(5, r, g, b, 1);	//set right LED
	    } else if (val == 5) {
		WS2812B_setPixelColorRGB(4, r, g, b, 1);	//set right 2 LEDs
		WS2812B_setPixelColorRGB(5, r, g, b, 1);
	    } else if (val == 6) {
		WS2812B_setPixelColorRGB(3, r, g, b, 1);	//set left LED
	    } else if (val == 7) {
		WS2812B_setPixelColorRGB(3, r, g, b, 1);	//set left 2 LEDs
		WS2812B_setPixelColorRGB(4, r, g, b, 1);
	    } else{
		WS2812B_setPixelColorRGB(3, 0, 0, 0, 1);	//set all LEDs off
		WS2812B_setPixelColorRGB(4, 0, 0, 0, 1);
		WS2812B_setPixelColorRGB(5, 0, 0, 0, 1);
	    }
	    break;
	case 3 :
	    if (val == 1) {					
		WS2812B_setPixelColorRGB(7, r, g, b, 1);	//set middle LED
	    } else if (val == 2) {					
		WS2812B_setPixelColorRGB(6, r, g, b, 1);	//set two outside LEDs
		WS2812B_setPixelColorRGB(8, r, g, b, 1);
	    } else if (val == 3) {
		WS2812B_setPixelColorRGB(6, r, g, b, 1);	//set all LEDs
		WS2812B_setPixelColorRGB(7, r, g, b, 1);
		WS2812B_setPixelColorRGB(8, r, g, b, 1);
	    } else if (val == 4) {
		WS2812B_setPixelColorRGB(8, r, g, b, 1);	//set right LED
	    } else if (val == 5) {
		WS2812B_setPixelColorRGB(7, r, g, b, 1);	//set right 2 LEDs
		WS2812B_setPixelColorRGB(8, r, g, b, 1);
	    } else if (val == 6) {
		WS2812B_setPixelColorRGB(6, r, g, b, 1);	//set left LED
	    } else if (val == 7) {
		WS2812B_setPixelColorRGB(6, r, g, b, 1);	//set left 2 LEDs
		WS2812B_setPixelColorRGB(7, r, g, b, 1);
	    } else{
		WS2812B_setPixelColorRGB(6, 0, 0, 0, 1);	//set all LEDs off
		WS2812B_setPixelColorRGB(7, 0, 0, 0, 1);
		WS2812B_setPixelColorRGB(8, 0, 0, 0, 1);
	    }
	    break;			
	case 4 :
	    if (val == 1) {					
		WS2812B_setPixelColorRGB(19, r, g, b, 1);	//set middle LED
	    } else if (val == 2) {					
		WS2812B_setPixelColorRGB(18, r, g, b, 1);	//set two outside LEDs
		WS2812B_setPixelColorRGB(20, r, g, b, 1);
	    } else if (val == 3) {
		WS2812B_setPixelColorRGB(18, r, g, b, 1);	//set all LEDs
		WS2812B_setPixelColorRGB(19, r, g, b, 1);
		WS2812B_setPixelColorRGB(20, r, g, b, 1);
	    } else if (val == 4) {
		WS2812B_setPixelColorRGB(18, r, g, b, 1);	//set right LED
	    } else if (val == 5) {
		WS2812B_setPixelColorRGB(18, r, g, b, 1);	//set right 2 LEDs
		WS2812B_setPixelColorRGB(19, r, g, b, 1);
	    } else if (val == 6) {
		WS2812B_setPixelColorRGB(20, r, g, b, 1);	//set left LED
	    } else if (val == 7) {
		WS2812B_setPixelColorRGB(19, r, g, b, 1);	//set left 2 LEDs
		WS2812B_setPixelColorRGB(20, r, g, b, 1);
	    } else{
		WS2812B_setPixelColorRGB(18, 0, 0, 0, 1);	//set all LEDs off
		WS2812B_setPixelColorRGB(19, 0, 0, 0, 1);
		WS2812B_setPixelColorRGB(20, 0, 0, 0, 1);
	    }
	    break;
	case 5 :
	    if (val == 1) {					
		WS2812B_setPixelColorRGB(16, r, g, b, 1);	//set middle LED
	    } else if (val == 2) {					
		WS2812B_setPixelColorRGB(15, r, g, b, 1);	//set two outside LEDs
		WS2812B_setPixelColorRGB(17, r, g, b, 1);
	    } else if (val == 3) {
		WS2812B_setPixelColorRGB(15, r, g, b, 1);	//set all LEDs
		WS2812B_setPixelColorRGB(16, r, g, b, 1);
		WS2812B_setPixelColorRGB(17, r, g, b, 1);
	    } else if (val == 4) {
		WS2812B_setPixelColorRGB(15, r, g, b, 1);	//set right LED
	    } else if (val == 5) {
		WS2812B_setPixelColorRGB(15, r, g, b, 1);	//set right 2 LEDs
		WS2812B_setPixelColorRGB(16, r, g, b, 1);
	    } else if (val == 6) {
		WS2812B_setPixelColorRGB(17, r, g, b, 1);	//set left LED
	    } else if (val == 7) {
		WS2812B_setPixelColorRGB(16, r, g, b, 1);	//set left 2 LEDs
		WS2812B_setPixelColorRGB(17, r, g, b, 1);
	    } else{
		WS2812B_setPixelColorRGB(15, 0, 0, 0, 1);	//set all LEDs off
		WS2812B_setPixelColorRGB(16, 0, 0, 0, 1);
		WS2812B_setPixelColorRGB(17, 0, 0, 0, 1);
	    }
	    break;			
	case 6 :
		if (val == 1) {					
		    WS2812B_setPixelColorRGB(13, r, g, b, 1);	//set middle LED
		} else if (val == 2) {					
		    WS2812B_setPixelColorRGB(12, r, g, b, 1);	//set two outside LEDs
		    WS2812B_setPixelColorRGB(14, r, g, b, 1);
		} else if (val == 3) {
		    WS2812B_setPixelColorRGB(12, r, g, b, 1);	//set all LEDs
		    WS2812B_setPixelColorRGB(13, r, g, b, 1);
		    WS2812B_setPixelColorRGB(14, r, g, b, 1);
		} else if (val == 4) {
		    WS2812B_setPixelColorRGB(12, r, g, b, 1);	//set right LED
		} else if (val == 5) {
		    WS2812B_setPixelColorRGB(12, r, g, b, 1);	//set right 2 LEDs
		    WS2812B_setPixelColorRGB(13, r, g, b, 1);
		} else if (val == 6) {
		    WS2812B_setPixelColorRGB(14, r, g, b, 1);	//set left LED
		} else if (val == 7) {
		    WS2812B_setPixelColorRGB(13, r, g, b, 1);	//set left 2 LEDs
		    WS2812B_setPixelColorRGB(14, r, g, b, 1);
		} else{
		    WS2812B_setPixelColorRGB(12, 0, 0, 0, 1);	//set all LEDs off
		    WS2812B_setPixelColorRGB(13, 0, 0, 0, 1);
		    WS2812B_setPixelColorRGB(14, 0, 0, 0, 1);
		}
		break;				
	case 7 :
		if (val == 1) {					
		    WS2812B_setPixelColorRGB(10, r, g, b, 1);	//set middle LED
		} else if (val == 2) {					
		    WS2812B_setPixelColorRGB(9, r, g, b, 1);	//set two outside LEDs
		    WS2812B_setPixelColorRGB(11, r, g, b, 1);
		} else if (val == 3) {
		    WS2812B_setPixelColorRGB(9, r, g, b, 1);	//set all LEDs
		    WS2812B_setPixelColorRGB(10, r, g, b, 1);
		    WS2812B_setPixelColorRGB(11, r, g, b, 1);
		} else if (val == 4) {
		    WS2812B_setPixelColorRGB(9, r, g, b, 1);	//set right LED
		} else if (val == 5) {
		    WS2812B_setPixelColorRGB(9, r, g, b, 1);	//set right 2 LEDs
		    WS2812B_setPixelColorRGB(10, r, g, b, 1);
		} else if (val == 6) {
		    WS2812B_setPixelColorRGB(11, r, g, b, 1);	//set left LED
		} else if (val == 7) {
		    WS2812B_setPixelColorRGB(10, r, g, b, 1);	//set left 2 LEDs
		    WS2812B_setPixelColorRGB(11, r, g, b, 1);
		} else{
		    WS2812B_setPixelColorRGB(9, 0, 0, 0, 1);	//set all LEDs off
		    WS2812B_setPixelColorRGB(10, 0, 0, 0, 1);
		    WS2812B_setPixelColorRGB(11, 0, 0, 0, 1);
		}
		break;				
	default :
	    //Throw exception
	    NRF_LOG_ERROR("\nTrying to set an unknown LED compartment.\n");
    }
}

void driveCompartmentLEDs_NEO(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7, uint8_t r, uint8_t g, uint8_t b) {
    ps_pixels_t strip;
    ps_pixels_init(&strip);
    setCompartmentLEDs_NEO(1, c1, r, g, b);
    setCompartmentLEDs_NEO(2, c2, r, g, b);
    setCompartmentLEDs_NEO(3, c3, r, g, b);
    setCompartmentLEDs_NEO(4, c4, r, g, b);
    setCompartmentLEDs_NEO(5, c5, r, g, b);
    setCompartmentLEDs_NEO(6, c6, r, g, b);
    setCompartmentLEDs_NEO(7, c7, r, g, b);
    WS2812B_show_pixels();
    ps_pixels_close();
}

void driveAppCompartmentLEDs_NEO(uint32_t leds_data, uint8_t r, uint8_t g, uint8_t b) {
    // Enable 5V supply for LEDs
    dc2_enable();
    
    // Assumes leds_data is formatted as 0xX-C7-C6-C5-C4-C3-C2-C1, with Cx ranging from 0-3
    driveCompartmentLEDs_NEO((leds_data & 0x0000000f)>>0, (leds_data & 0x000000f0)>>4, (leds_data & 0x00000f00)>>8, (leds_data & 0x0000f000)>>12, (leds_data & 0x000f0000)>>16, (leds_data & 0x00f00000)>>20, (leds_data & 0x0f000000)>>24, r, g, b);
}

void blinkCompartmentLEDs_NEO(uint8_t num_blink, uint32_t ms_delay, uint8_t r, uint8_t g, uint8_t b) {	
    for (uint8_t ii=0; ii<num_blink; ii++) {	
	driveAppCompartmentLEDs_NEO(0x03333333, r, g, b);
	nrf_delay_ms(ms_delay);
	driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
	nrf_delay_ms(ms_delay);
    }
}

void blinkOffsetCompartmentLEDs_NEO(uint8_t num_blink, uint32_t ms_delay) {
    uint8_t r1 = g_alarm_led_r;
    uint8_t g1 = g_alarm_led_g;
    uint8_t b1 = g_alarm_led_b;
    uint8_t r2 = g_alarm_led_r;
    uint8_t g2 = g_alarm_led_g;
    uint8_t b2 = g_alarm_led_b;
    for (uint8_t ii=0; ii<num_blink; ii++) {	
	driveAppCompartmentLEDs_NEO(0x01111111, r1, g1, b1);
	nrf_delay_ms(ms_delay);
	driveAppCompartmentLEDs_NEO(0x02222222, r2, g2, b2);
	nrf_delay_ms(ms_delay);
    }
    driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
}

void colorWipe_NEO(uint8_t r, uint8_t g, uint8_t b, uint8_t ms_delay) {
    ps_pixels_t strip;
    ps_pixels_init(&strip);
    for (uint8_t ii=0; ii<NUM_COMP_LEDS; ii++) {
	WS2812B_setPixelColorRGB(ii, r, g, b, 1);
	WS2812B_show_pixels();
	nrf_delay_ms(ms_delay);
    }
    for (uint8_t jj=0; jj<NUM_COMP_LEDS; jj++) {
	WS2812B_setPixelColorRGB(jj, 0, 0, 0, 1);
	WS2812B_show_pixels();
	nrf_delay_ms(ms_delay);
    }
    ps_pixels_close();
}

void eg_set_single_led(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
    ps_pixels_t strip;
    ps_pixels_init(&strip);
    WS2812B_setPixelColorRGB(n, r, g, b, 1);
    WS2812B_show_pixels();
    ps_pixels_close();
}

void eg_leds_animate_select(uint8_t select_led_comp, uint8_t select_led_pattern) {
    l_select_led_comp = select_led_comp;
    l_select_led_pattern = select_led_pattern;
    
    // Stop existing LED animation timer?
    eg_leds_stop_led_select_timer();
	
    if (select_led_pattern == LED_SEL_PATTERN_SNAKE) {
	eg_leds_start_led_select_timer(g_led_select_snake_duration_ms);
    } else if (g_time_invalid == true) {
	eg_leds_start_led_select_timer(LED_SELECT_RESET_DURATION_MS);
    } else {
	eg_leds_start_led_select_timer(g_led_select_duration_ms);
    }
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
		driveAppCompartmentLEDs_NEO(0x1<<((l_select_led_comp-1)*4), g_alarm_led_r, g_alarm_led_g, g_alarm_led_b);
	    } else {
		driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
	    }
	    break;

	case LED_SEL_PATTERN_3ON3OFF:
	    if (led_select_count % 2) {
		driveAppCompartmentLEDs_NEO(0x3<<((l_select_led_comp-1)*4), g_alarm_led_r, g_alarm_led_g, g_alarm_led_b);
	    } else {
		driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
	    }
	    break;

	case LED_SEL_PATTERN_123OFF:
	    blink_state = led_select_count % 4;
	    if (blink_state == 1) {
		driveAppCompartmentLEDs_NEO(0x6<<((l_select_led_comp-1)*4), g_alarm_led_r, g_alarm_led_g, g_alarm_led_b);
	    } else if (blink_state == 2) {
		driveAppCompartmentLEDs_NEO(0x7<<((l_select_led_comp-1)*4), g_alarm_led_r, g_alarm_led_g, g_alarm_led_b);
	    } else if (blink_state == 3) {
		driveAppCompartmentLEDs_NEO(0x3<<((l_select_led_comp-1)*4), g_alarm_led_r, g_alarm_led_g, g_alarm_led_b);
	    }
	    else {
		driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
	    }
	    break;
		
	case LED_SEL_PATTERN_ALL1ONOFF:
	    if (g_time_invalid == true) {
		// Display flashing LEDs for reset
		eg_audio_off(DISABLE_AUDIO);
		if (led_select_count % 2) {
//                    driveAppCompartmentLEDs_NEO(0x01111111, RESET_LED_R, RESET_LED_G, RESET_LED_B);   // ~120mA
//                    driveAppCompartmentLEDs_NEO(0x00000111, RESET_LED_R, RESET_LED_G, RESET_LED_B); // ~70mA
//                    driveAppCompartmentLEDs_NEO(0x04000000, RESET_LED_R, RESET_LED_G, RESET_LED_B);   // ~50mA
		    driveAppCompartmentLEDs_NEO(0x00000006, RESET_LED_R, RESET_LED_G, RESET_LED_B);   // ~50mA
		} else {
		    driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
		}
	    } else {
		if (led_select_count % 2) {
		    driveAppCompartmentLEDs_NEO(0x01111111, g_alarm_led_r, g_alarm_led_g, g_alarm_led_b);
		} else {
		    driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
		}
	    }
		break;
		
	case LED_SEL_PATTERN_ALL3ONOFF:
	    if (led_select_count % 2) {
		driveAppCompartmentLEDs_NEO(0x03333333, g_alarm_led_r, g_alarm_led_g, g_alarm_led_b);
	    } else {
		driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
	    }
	    break;

	case LED_SEL_PATTERN_ALL123:
	    blink_state = led_select_count % 4;
	    if (blink_state == 1) {
		driveAppCompartmentLEDs_NEO(0x06666666, g_alarm_led_r, g_alarm_led_g, g_alarm_led_b);
	    } else if (blink_state == 2) {
		driveAppCompartmentLEDs_NEO(0x07777777, g_alarm_led_r, g_alarm_led_g, g_alarm_led_b);
	    } else if (blink_state == 3) {
		driveAppCompartmentLEDs_NEO(0x03333333, g_alarm_led_r, g_alarm_led_g, g_alarm_led_b);
	    }
	    else {
		driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
	    }
	    break;
		
	case LED_SEL_PATTERN_SNAKE:
	    eg_set_single_led((led_select_count % NUM_COMP_LEDS), g_alarm_led_r, g_alarm_led_g, g_alarm_led_b);
	    break;

	default:
	    // no implementation
	    break;
    }
    
    led_select_count++;
}


void display_led_battery_level(uint8_t led_bat_lvl) {
//    ps_pixels_t strip;
//    ps_pixels_init(&strip);
//
//    // Enable DC2 and disable audio
//    eg_audio_off(DISABLE_AUDIO);
//    
//    // Display all 10 LEDs first in RED
//    uint8_t ii, jj;
//    for (ii=(BAT_LEVEL_LED_1-1); ii>=(BAT_LEVEL_LED_10-1); ii--) {
//	WS2812B_setPixelColorRGB(ii, RESET_LED_R, RESET_LED_G, RESET_LED_B, 1);
//    }
//    WS2812B_show_pixels();
//    nrf_delay_ms(300);
//    
//    // Display increasing battery level animation in GREEN
//    for (ii=(BAT_LEVEL_LED_1-1), jj=1; ii>=(BAT_LEVEL_LED_10-1); ii--, jj++) {
//	if (jj <= led_bat_lvl) {
//	    WS2812B_setPixelColorRGB(ii, DEFAULT_ALARM_LED_R, DEFAULT_ALARM_LED_B, DEFAULT_ALARM_LED_G, 1);
//	    WS2812B_show_pixels();
//	    nrf_delay_ms(100);
//	}
//    }
//    WS2812B_show_pixels();
//    ps_pixels_close();

    // Show battery level for 3 seconds
    eg_leds_start_battery_display_timer(BATTERY_DISPLAY_TIME_MS);
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

void eg_leds_start_battery_display_timer(uint32_t timer_ms) {
    APP_ERROR_CHECK(app_timer_start(m_battery_display_timer_id, APP_TIMER_TICKS(timer_ms), NULL));
}

void eg_leds_stop_battery_display_timer(void) {
    APP_ERROR_CHECK(app_timer_stop(m_battery_display_timer_id));
}

void battery_display_timer_handler(void * p_context) {
    eg_leds_off();
}

void eg_leds_start_led_off_timer(uint32_t timer_ms) {
    APP_ERROR_CHECK(app_timer_start(m_led_off_timer_id, APP_TIMER_TICKS(timer_ms), NULL));
}

void eg_leds_stop_led_off_timer(void) {
    APP_ERROR_CHECK(app_timer_stop(m_led_off_timer_id));
}

void led_off_timer_handler(void * p_context) {
//    // Turn off LEDs during heart beat (while alarm is triggered)
//    if (g_alarm_status == ALARM_DATA_INVALID) {
//        driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
//    }
    driveAppCompartmentLEDs_NEO(0, 0, 0, 0);
}
