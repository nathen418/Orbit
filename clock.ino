
//Global libraries
#include <TimeLib.h>  
#include <Wire.h>
#include <RTClib.h>
#include <PinChangeInterrupt.h>
//Local libraries
#include "sevenseg_display_drv.h"

//Define section
#define BUTTON_PLUS 12
#define BUTTON_MINUS 11
#define BUTTON_MODE 10
#define BUTTON_SET 13
#define MAX_MODE 2
#define SECS_PER_HOUR 3600


//Top variables
sevenseg_display_drv display7seg;
double light;
volatile uint8_t mode,stopwatch_running,mode_button_pressed;
volatile unsigned long last_time_button_falling,last_time_button_rising,elapsed_millisec,stopwatch_start_millisec;
//unsigned long 

RTC_DS3231 rtc;

void loop(){
	switch (mode) {
		case 0:
			mode=1;
			display7seg.clear_all();
			display7seg.set_dp_bottom(2);
			break;
		case 1://Clock with date
			show_clock_up();
			show_date_down();
			set_brightness();
			break;
		case 10://Clock with year
			display7seg.clear_all();
			mode=11;
			break;
		case 11://Clock with year
			show_clock_up();
			show_year_down();
			set_brightness();
			break;
		case 20://Stop watch
			mode=21;
			display7seg.clear_all();
			display7seg.set_colon_top();
			display7seg.set_dp_bottom(2);
			break;
		case 21://Stop watch
			stopwatch();
			break;
		default:
			mode=0;
			break;
	}//switch
}

void stopwatch(){
	//Stopwatch function	
	if(stopwatch_running==1) elapsed_millisec=millis()-stopwatch_start_millisec;
	
	uint16_t total_sec=elapsed_millisec/1000;
	uint16_t delata_millisec=elapsed_millisec%1000;
	uint8_t delta_sec=total_sec%60;
	uint8_t delta_min=total_sec/60%60;
	uint8_t delta_hour=total_sec/60/60;
	
	uint8_t digit;
	digit=elapsed_millisec/10%10;
	display7seg.set_digit_bottom(0, digit);
	digit=elapsed_millisec/100%10;
	display7seg.set_digit_bottom(1, digit);
	digit=delta_sec%10;
	display7seg.set_digit_bottom(2, digit);
	digit=delta_sec/10;
	display7seg.set_digit_bottom(3, digit);
	
	digit=delta_min%10;
	display7seg.set_digit_top(0, digit);
	digit=delta_min/10;
	display7seg.set_digit_top(1, digit);
	digit=delta_hour%10;
	display7seg.set_digit_top(2, digit);
	digit=delta_hour/10;
	display7seg.set_digit_top(3, digit);
}

void set_brightness(){
	light = 0;
	for (int i = 0; i < 2000; i++) {
		light += analogRead(A3);
	}

	light = int((light/ 2000) / 1024 * 15);
	display7seg.set_brightness(light);
}

void show_clock_up(){
	display7seg.set_digit_top(0, minute()%10);
	display7seg.set_digit_top(1, minute()/10);
	display7seg.set_digit_top(2, hour()%10);
	display7seg.set_digit_top(3, hour()/10);
	
	if (millis()%1000 > 500) display7seg.set_colon_top();
	else display7seg.reset_colon_top();
}

void show_date_down(){
	display7seg.set_digit_bottom(0, month()%10);
	display7seg.set_digit_bottom(1, month()/10);
	display7seg.set_digit_bottom(2, day()%10);
	display7seg.set_digit_bottom(3, day()/10);
}

void show_year_down(){
	uint8_t digit;
	
	digit=year()%10;
	display7seg.set_digit_bottom(0, digit);
	digit=year()/10%10;
	display7seg.set_digit_bottom(1, digit);
	digit=year()/100%10;
	display7seg.set_digit_bottom(2, digit);
	digit=year()/1000%10;
	display7seg.set_digit_bottom(3, digit);
}

void print_float_num_bottom(double floatnum) {
	char tempString[5];
	int fourdigits;

	display7seg.reset_dp_bottom(0);
	display7seg.reset_dp_bottom(1);
	display7seg.reset_dp_bottom(2);
	display7seg.reset_dp_bottom(3);
	
	int dig_num = log10(floatnum) + 1;
	if (dig_num >= 0) {
		fourdigits = round(floatnum*pow(10, 4 - dig_num));
		if (fourdigits==0) sprintf(tempString, "%4s", "0000");
		else sprintf(tempString, "%4d", fourdigits);
		display7seg.set_digit_bottom(0, (uint8_t)(tempString[3] - '0'));
		display7seg.set_digit_bottom(1, (uint8_t)(tempString[2] - '0'));
		display7seg.set_digit_bottom(2, (uint8_t)(tempString[1] - '0'));
		display7seg.set_digit_bottom(3, (uint8_t)(tempString[0] - '0'));
		display7seg.set_dp_bottom(4 - dig_num);
	}
	else {
		fourdigits = round(floatnum*10000);
		sprintf(tempString, "%4d", fourdigits);
		display7seg.set_digit_bottom(0, fourdigits % 10 );
		display7seg.set_digit_bottom(1, fourdigits /10 %10);
		display7seg.set_digit_bottom(2, (fourdigits /100 % 10));
		display7seg.set_digit_bottom(3, 0);
		display7seg.set_dp_bottom(3);
	}
}

//http://forum.arduino.cc/index.php?topic=44262.0
char *ftoa(char *a, double f, int precision)
{
	long p[] = { 0,10,100,1000,10000,100000,1000000,10000000,100000000 };

	char *ret = a;
	long heiltal = (long)f;
	itoa(heiltal, a, 10);
	while (*a != '\0') a++;
	*a++ = '.';
	long desimal = abs((long)((f - heiltal) * p[precision]));
	itoa(desimal, a, 10);
	return ret;
}
