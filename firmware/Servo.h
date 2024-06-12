#include <ESP32Servo.h>

#define WIPER_MIN_R 722
#define WIPER_MAX_R 2462

#define WIPER_MIN_L 620
#define WIPER_MAX_L 2399

#define PIN_SERVO_RIGHT 13
#define PIN_SERVO_LEFT 27

Servo wiper_left;
Servo wiper_right;

float wiper_dir_left = 1.0;
float wiper_dir_right = 1.0;

float wiper_state_left = 0.5;
float wiper_state_right = 0.5;

float servo_pos_left = 0.0;
float servo_pos_right = 0.0;

unsigned long L_servo_update_time = millis();
unsigned long R_servo_update_time = millis();

void initServos(){
	wiper_left.setTimerWidth(20);
	wiper_left.attach(PIN_SERVO_LEFT, WIPER_MIN_L, WIPER_MAX_L);
	wiper_left.writeMicroseconds((WIPER_MIN_L + WIPER_MAX_L)/2);

	wiper_right.setTimerWidth(20);
	wiper_right.attach(PIN_SERVO_RIGHT, WIPER_MIN_R, WIPER_MAX_R);
	wiper_right.writeMicroseconds((WIPER_MIN_R + WIPER_MAX_R)/2);
}

extern float L_swipe_speed;
extern float R_swipe_speed;

extern int L_angle_min;
extern int L_angle_max;

extern int R_angle_min;
extern int R_angle_max;

float degSecToRaw(float degreespersec){
	return degreespersec * 0.02 * 0.00625;
}

void updateServoRight(){

	if(wiper_state_right < 0.0){
		wiper_state_right = 0.0;
	}
	else if(wiper_state_right > 1.0){
		wiper_state_right = 1.0;
	}

	int finalstate_right = int(float(WIPER_MIN_R) + (1.0 - wiper_state_right) * float(WIPER_MAX_R - WIPER_MIN_R));
	wiper_right.writeMicroseconds(finalstate_right);

	R_servo_update_time = millis();
}

void updateServoLeft(){

	if(wiper_state_left < 0.0){
		wiper_state_left = 0.0;
	}
	else if(wiper_state_left > 1.0){
		wiper_state_left = 1.0;
	}

	int finalstate_left = int(float(WIPER_MIN_L) + (1.0 - wiper_state_left) * float(WIPER_MAX_L - WIPER_MIN_L));
	wiper_left.writeMicroseconds(finalstate_left);

	L_servo_update_time = millis();
}

void updateSwipeLeft(){

	if(millis() - L_servo_update_time < 20)
		return;
	
	wiper_state_left += wiper_dir_left * degSecToRaw(L_swipe_speed);

	if(wiper_state_left > L_angle_max/100.0){
		wiper_state_left = L_angle_max/100.0;
		wiper_dir_left = -1.0;
	}
	else if(wiper_state_left < L_angle_min/100.0){
		wiper_state_left = L_angle_min/100.0;
		wiper_dir_left = 1.0;
	}

	updateServoLeft();
}

void updateSwipeRight(){

	if(millis() - R_servo_update_time < 20)
		return;
	
	wiper_state_right += wiper_dir_right * degSecToRaw(R_swipe_speed);

	if(wiper_state_right > R_angle_max/100.0){
		wiper_state_right = R_angle_max/100.0;
		wiper_dir_right = -1.0;
	}
	else if(wiper_state_right < R_angle_min/100.0){
		wiper_state_right = R_angle_min/100.0;
		wiper_dir_right = 1.0;
	}

	updateServoRight();
}

float processStatic(float state, int tgt){
	if(state > (float(tgt)/100.0) + 0.002){
		return state - 0.005;
	}
	
	if(state < (float(tgt)/100.0) - 0.002){
		return state + 0.005;
	}

	return state;
}

void updateStaticRight(int servoangle){
	
	if(millis() - R_servo_update_time < 20)
		return;

	wiper_state_right = processStatic(wiper_state_right, servoangle);
	updateServoRight();
}

void updateStaticLeft(int servoangle){
	
	if(millis() - L_servo_update_time < 20)
		return;

	wiper_state_left = processStatic(wiper_state_left, servoangle);
	updateServoLeft();
}
