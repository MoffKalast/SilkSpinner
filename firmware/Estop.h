#define PIN_ESTOP 18

unsigned long update_time_estop = millis();

extern float motor_vel_right;
extern float motor_vel_left;

extern int L_throttle;
extern int R_throttle;

extern bool R_start;
extern bool L_start;

void initESTOP(){
	pinMode(PIN_ESTOP, INPUT_PULLUP);
}

void stopAll(){
	motor_vel_right = 0.0;
	motor_vel_left = 0.0;

	L_throttle = 0;
	R_throttle = 0;

	R_start = 0;
	L_start = 0;

	setings_updated = true;
}

void checkESTOP(){

	if(millis() - update_time_estop < 2){
		return;
	}
	
	if(digitalRead(PIN_ESTOP) == 0 && !setings_updated){
		stopAll();
	}

	update_time_estop = millis();
}
