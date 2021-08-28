#include "sevenseg_display_drv.h"

static sevenseg_display_drv *active7segment = NULL;

sevenseg_display_drv::sevenseg_display_drv() { }

void sevenseg_display_drv::begin_drv()
{
	//Start refreshing Nothing will be displayed before this function called
	//From http://www.instructables.com/id/Arduino-Timer-Interrupts/
	//Set timer1 - 16 bit timer
	
	cli();// disable global interrupts
	TCCR1A = 0;// set entire TCCR1A register to 0
	TCCR1B = 0;// same for TCCR1B
	TCNT1 = 0;//initialize counter value to 0

	//Prescaller (timer speed(Hz)) = (Arduino clock speed(16MHz)) / prescaler
	TCCR1B |= (1 << CS10); //No prescaller 63ns per count for 16MHz clock
	
	//Also notice how the setups between the three timers differ slightly in the line which turns on CTC mode :
	//TCCR0A |= (1 << WGM01);//for timer0
	//TCCR1B |= (1 << WGM12);//for timer1
	//TCCR2A |= (1 << WGM21);//for timer2
	TCCR1B |= (1 << WGM12);//CTC mode

	// OCR#A (the compare match value)
	//remember that when you use timers 0 and 2 this number must be less than 256, and less than 65536 for timer1
	OCR1A = 999;//intrupt will happen 63ns*[this value] 63usec for 999
	//OCR1A = 496;//31usec
	TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
	sei();//allow interrupts
}

ISR(TIMER1_COMPA_vect) {//change the 1 to 0 for timer0 and 2 for timer2
	active7segment->updateDisplay();//Matrix refrshing function
}

uint8_t sevenseg_display_drv::init()
{
	//Initialisation
	DDRB |= B00000011;//Set pins 0,1 of port B as output
	DDRD |= B11111111;//Sets pins 0 to 7 of port D as outputs, segments control
	DDRC |= B00000111;//Set bits 0,1,2 of port C as output, connection to 74HC595N


	PORTB &= B11111100;//Turn off colon and apostrof
	PORTD =  B00000000;//Turn off all segments
	PORTC &= B11111000;//Shift register controls low
	
	//Clear all bits of shift register
	for (uint8_t n = 0; n < NUMOFDIGITS; n++) {
		PORTC |= B00000010;//Serial clock high
		PORTC &= B11111101;//Serial clock low
	}
	PORTC |= B00000100;//Latch shift register data

	active7segment = this; // For interrupt hander

	//Clear memory
	clear_all();

	//For pwm_count_max=15 each digit can be turn on up to 1ms out of 8 ms
	//So reftesh rate something like 125Hz
	//pwm_count_max = 15;//Number of brightness levels
	brightness = 15;//Default value of brightness

	//Initiate variables
	pwm_count = 0;
	digit_num = 0;
	return 0;
}

void sevenseg_display_drv::clear_all(){
	//Clear memory
	for (uint8_t n = 0; n < NUMOFDIGITS; n++) {
		dp_buffer[n] =   B00000000;//Reset decimal point buffer
		displaybuff[n] = B00000000;//Set all digits to off
		colon_and_ap[n]= B00000000;//Clean colon and apos
	}
}

void sevenseg_display_drv::clear_top(){
	//Clear memory
	for (uint8_t n = 4; n < NUMOFDIGITS; n++) {
		dp_buffer[n] =   B00000000;//Reset decimal point buffer
		displaybuff[n] = B00000000;//Set all digits to off
		colon_and_ap[n]= B00000000;//Clean colon and apos
	}
}

void sevenseg_display_drv::clear_bottom(){
	//Clear memory
	for (uint8_t n = 0; n < 4; n++) {
		dp_buffer[n] =   B00000000;//Reset decimal point buffer
		displaybuff[n] = B00000000;//Set all digits to off
		colon_and_ap[n]= B00000000;//Clean colon and apos
	}
}

void sevenseg_display_drv::updateDisplay()
{
	/*
	This function refreshing display. Called only from interupt
	*/
	
	//Debug
	//PORTC |= B00000001;//Turn off all digits

	if (pwm_count < PWMCOUNTMAX) {
		//For brightness=0 display will be on for 1 cycle
		if (pwm_count > brightness) {
			PORTB &= B11111100;//Turn off colon and apostrof
			PORTD =  B00000000;//Turn off all segments
		}
		pwm_count++;//PWM counter
	}
	else {//End of current pwm cycle
		//Here we need to update information of digit
		pwm_count = 0;//restart pwm counter
		//Turn off all
		PORTB &= B11111100;//Turn off colon and apostrof
		PORTD  = B00000000;//Turn off all segments

		if (digit_num < NUMOFDIGITS) {// Digits 1-7
			PORTC &= B11111000;//Shift register latch data and clock signal low
			PORTC |= B00000010;//Serial clock high
			PORTC |= B00000101;//Latch data, serial data to high
		}
		else {//Digit 0
			digit_num = 0;//First digit
			//Turn on first digit
			PORTC &= B11111001;//Shift register latch and clock signal low. Data already high vrom previous cycle
			PORTC |= B00000010;//Serial clock high
			PORTC |= B00000100;//Latch data
		}
		//Ser digit data
		PORTD = displaybuff[digit_num];//Apply data for segments of current digit
		PORTB |= colon_and_ap[digit_num];//Colon and apostrophe 
		digit_num++;//Next digit
	}//end else
	
	//*** Debug ***
	//long elapsed = micros() - time_start;
	//Serial.println(elapsed);
	//PORTC &= B11111110;//Turn off all digits
	//********************
}

//Set value of digit
void sevenseg_display_drv::set_digit( uint8_t dig_num, uint8_t value){
	if(dig_num<0 || dig_num>(NUMOFDIGITS-1)) return;
	displaybuff[dig_num]=pgm_read_byte_near(&bcd[value])|dp_buffer[dig_num];
}

void sevenseg_display_drv::set_digit_top( uint8_t dig_num, uint8_t value){
	set_digit( dig_num+4, value);
}

void sevenseg_display_drv::set_digit_bottom( uint8_t dig_num, uint8_t value){
	set_digit( 3-dig_num, value);
}

//Set brightness of all digits
void sevenseg_display_drv::set_brightness(uint8_t value) {
	if (value<= PWMCOUNTMAX) brightness = value;
	else brightness = PWMCOUNTMAX;
}


void sevenseg_display_drv::set_dp_top(uint8_t dig_num) {
	if(dig_num<0 || dig_num>(NUMOFDIGITS-1)) return;
	dp_buffer[dig_num+4] = B10000000;
	displaybuff[dig_num+4] |= dp_buffer[dig_num+4];
}

void sevenseg_display_drv::set_dp_bottom(uint8_t dig_num) {
	if(dig_num<0 || dig_num>(NUMOFDIGITS-1)) return;
	dp_buffer[3-dig_num] = B10000000;
	displaybuff[3-dig_num] |= dp_buffer[3-dig_num];
}

void sevenseg_display_drv::reset_dp_top(uint8_t dig_num) {
	if(dig_num<0 || dig_num>(NUMOFDIGITS-1)) return;
	dp_buffer[dig_num] = B00000000;
	displaybuff[dig_num] &= B01111111;
}

void sevenseg_display_drv::reset_dp_bottom(uint8_t dig_num) {
	if(dig_num<0 || dig_num>(NUMOFDIGITS-1)) return;
	dp_buffer[dig_num] = B00000000;
	displaybuff[dig_num] &= B01111111;
}

void sevenseg_display_drv::set_colon_top() {
	for (uint8_t n = 4; n < 8; n++) {
		colon_and_ap[n] |=B00000010;
	}
}

void sevenseg_display_drv::set_colon_bottom() {
	for (uint8_t n = 0; n < 4; n++) {
		colon_and_ap[n] |=B00000010;
	}
}

void sevenseg_display_drv::reset_colon_top() {
	for (uint8_t n = 4; n < 8; n++) {
		colon_and_ap[n] &=B11111101;
	}
}

void sevenseg_display_drv::reset_colon_bottom() {
	for (uint8_t n = 0; n < 4; n++) {
		colon_and_ap[n] &=B11111101;
	}
}

void sevenseg_display_drv::set_apostrophe_top() {
	for (uint8_t n = 4; n < 8; n++) {
		
	}
}

void sevenseg_display_drv::set_apostrophe_bottom() {
	for (uint8_t n = 0; n < 4; n++) {
		
	}
}

void sevenseg_display_drv::reset_apostrophe_top() {
	for (uint8_t n = 4; n < 8; n++) {
		
	}
}

void sevenseg_display_drv::reset_apostrophe_bottom() {
	for (uint8_t n = 0; n < 4; n++) {
		
	}
}