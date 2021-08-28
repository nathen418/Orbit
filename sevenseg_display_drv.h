//Used information:
//http://www.allaboutcircuits.com/projects/interface-a-seven-segment-display-to-an-arduino/
//http://skpang.co.uk/blog/archives/323



#ifndef _SEVENSEG_DISPLAY_DRV_h
#define _SEVENSEG_DISPLAY_DRV_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
	#include "pins_arduino.h"
#endif

#define NUMOFDIGITS 8
#define PWMCOUNTMAX 15 //Number of brightness levels

//Connections:
//Port D:a-0, b-1,c-2,d-3,e-4,f-5,g-6,7-dp
//Port B 1 - col,0 - apos
//Port C 0 - SER,1 - CLK,2 - RCLK connection to 74HC595N
/*
const unsigned char PROGMEM bcd[10] = {
	0xC0, //0
	0xF9, //1
	0xA4, //2
	0xB0, //3
	0x99, //4
	0x92, //5
	0x82, //6
	0xF8, //7
	0x80, //8
	0x90 //9
};
*/

const unsigned char PROGMEM bcd[10] = {
	~0xC0, //0
	~0xF9, //1
	~0xA4, //2
	~0xB0, //3
	~0x99, //4
	~0x92, //5
	~0x82, //6
	~0xF8, //7
	~0x80, //8
	~0x90 //9
};

class sevenseg_display_drv
{
public:
	sevenseg_display_drv();
	void begin_drv();
	void updateDisplay();
	uint8_t init();
	void set_digit( uint8_t dig_num, uint8_t value);
	void set_digit_top( uint8_t dig_num, uint8_t value);
	void set_digit_bottom( uint8_t dig_num, uint8_t value);
	void set_brightness(uint8_t brightness);
	void set_dp_top(uint8_t dig_num );
	void set_dp_bottom(uint8_t dig_num );
	void reset_dp_top(uint8_t dig_num);
	void reset_dp_bottom(uint8_t dig_num);
	void set_colon_top();
	void reset_colon_top();
	void set_apostrophe_top();
	void reset_apostrophe_top();
	void set_colon_bottom();
	void reset_colon_bottom();
	void set_apostrophe_bottom();
	void reset_apostrophe_bottom();
	void clear_all();
	void clear_top();
	void clear_bottom();
private:
	uint8_t         displaybuff[8],dp_buffer[8],colon_and_ap[8];//Supports up to 2 4 digit 7 segment modules
	volatile uint8_t pwm_count,brightness, digit_num,display_num, pwm_count_max;
};

#endif