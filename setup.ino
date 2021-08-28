//Arduino code not written by me, Nate
// I can't find my origional code or the author of this code so I am just
// going to upload this here in hopes that I come back
// to it at a later date

void setup()
{
	//Serial.begin(9600);
	display7seg.init();//Initiate displays
	display7seg.begin_drv();//Run interupt
	display7seg.set_brightness(15);
	if (! rtc.begin()) {
		Serial.println("Couldn't find RTC");
		while (1);
	}

	if (rtc.lostPower()) {
		Serial.println("RTC lost power, lets set the time!");
		// following line sets the RTC to the date & time this sketch was compiled
		rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
		// This line sets the RTC with an explicit date & time, for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
	}
	
	setSyncProvider(syncProvider);   // the function to get the time from the RTC
	if(timeStatus()!= timeSet) 
		Serial.println("Unable to sync with the RTC");
	else
		Serial.println("RTC has set the system time"); 
	
	pinMode(A3, INPUT);//Light sensor
	pinMode(BUTTON_PLUS, INPUT);
	pinMode(BUTTON_MINUS, INPUT);
	pinMode(BUTTON_MODE, INPUT);
	pinMode(BUTTON_SET, INPUT);
	
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(BUTTON_PLUS), plus_button, FALLING);
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(BUTTON_MINUS), minus_button, FALLING);
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(BUTTON_MODE), mode_button_down, FALLING);
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(BUTTON_SET), set_button, FALLING);
	
	last_time_button_falling=0;
	last_time_button_rising=0;
	mode = 0;
	stopwatch_running=0;
	mode_button_pressed=0;
}

static size_t syncProvider(){
	return rtc.now().unixtime();
}
