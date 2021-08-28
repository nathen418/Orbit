void plus_button(){

}

void minus_button(){

}

void set_button(){
	if((millis()-last_time_button_falling)<200) return;
	last_time_button_falling=millis();
	
	switch (mode) {
		case 1://Clock with date
			mode=10;
			break;
		case 11://Clock with year
			mode=0;
			break;
		case 21://Stop watch
			switch (stopwatch_running) {
				case 0://Stopwatch not running state
					//Run it
					stopwatch_running=1;
					stopwatch_start_millisec=millis();
					break;
				case 1://Stopwatch running state
					//Stop it
					stopwatch_running=2;
					break;
				case 2://Display results state
					//Reset it
					stopwatch_running=0;
					elapsed_millisec=0;
					break;
				default:
					break;
			}//switch stopwatch_running
		default:
			break;
	}//switch mode
}

void mode_button_down(){
	if((millis()-last_time_button_falling)<200) return;
	last_time_button_falling=millis();
	mode_button_pressed=1;
	switch (mode) {
		case 1://Clock with date
			mode=20;
			break;
		case 11://Clock with year
			mode=20;
			break;
		case 21://Stop watch
			mode=0;
			break;
		default:
			mode=0;
			break;
	}//switch mode
}

void mode_button_up(){
	if((millis()-last_time_button_rising)<200) return;
	last_time_button_rising=millis();
	mode_button_pressed=0;
	/*
	switch (mode) {
		case 1://Clock with date
			break;
		case 11://Clock with year
			break;
		case 21://Stop watch
			break;
		default:
			break;
	}//switch mode
	*/
}