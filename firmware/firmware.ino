#include "Motor.h"
#include "Servo.h"
#include "Hall.h"
#include "WebApp.h"
#include "Estop.h"

#include <math.h>

#define MODE_SWIPE 0
#define MODE_STATIC 1
#define MODE_GROUPS 2

unsigned long update_time = millis();

extern int counter_left;
extern int counter_right;

extern int L_throttle;
extern int R_throttle;

extern int L_groups;
extern int L_group_len;

extern int R_groups;
extern int R_group_len;

extern bool R_start;
extern bool L_start;

extern int L_angle;
extern int R_angle;

extern int L_angle_min;
extern int L_angle_max;

extern int R_angle_min;
extern int R_angle_max;

extern int L_mode;
extern int R_mode;

extern bool setings_updated;

extern float wiper_state_left;
extern float wiper_state_right;

extern float motor_vel_right;
extern float motor_vel_left;

int L_current_group = 0;
int L_count_zeroer = 0;
float L_group_progress = 0;
bool L_wait_for_servo = true;

int R_current_group = 0;
int R_count_zeroer = 0;
float R_group_progress = 0;
bool R_wait_for_servo = true;

void setup() {
	Serial.begin(115200);

	initMotors();
	initServos();
	initWebApp();
	initESTOP();
}

void updateSockets(){
	if(millis() - update_time > 300){
		sendTelemetry();
		update_time = millis();
		
	}
	else if(setings_updated){
		sendSettingsData();
		setings_updated = false;
	}	
}

int groupSplit(int i, int groups, int minval, int maxval){
	float total = float(maxval - minval);
	float delta = total/(float(groups) - 1.0);

	return round(minval + i*delta);
}

void loop() {

	readSensors();

	if(L_start)
	{
		if(L_mode == MODE_SWIPE){
			updateSwipeLeft();
			setMotorLeft(L_throttle);
		}
		else if(L_mode == MODE_STATIC){
			updateStaticLeft(L_angle);
			setMotorLeft(L_throttle);
		}
		else if(L_mode == MODE_GROUPS){

			if(L_current_group < L_groups)
			{
				if(L_group_progress > L_group_len - 0.06){
					L_current_group++;
					L_group_progress = 0;
					L_count_zeroer = counter_left;
					L_wait_for_servo = true;
					motor_vel_left = 0;
				}
				else{
					L_group_progress = (counter_left-L_count_zeroer) * 0.06;
				}

				int grouploc = groupSplit(L_current_group, L_groups, L_angle_min, L_angle_max);
				if(motor_vel_left == 0.0){
					updateStaticLeft(grouploc);		
				}

				if(L_wait_for_servo){
					setMotorLeft(0);
					if(fabs(float(grouploc)/100.0 - wiper_state_left) < 0.006){
						L_wait_for_servo = false;
					}
				}else if(L_group_progress > L_group_len - 0.5){
					setMotorLeft(L_throttle * 0.15);
				}else if(L_group_progress > L_group_len - 0.2){
					setMotorLeft(L_throttle * 0.03);
				}else{
					setMotorLeft(L_throttle);
				}
			}
			else{
				L_start = false;
				setings_updated = true;
				setMotorLeft(0);
			}
		}
	}
	else
	{
		if(L_mode == MODE_STATIC){
			updateStaticLeft(L_angle);
		}
		setMotorLeft(0);
	}

	if(R_start)
	{
		if(R_mode == MODE_SWIPE){
			updateSwipeRight();
			setMotorRight(R_throttle);
		}
		else if(R_mode == MODE_STATIC){
			updateStaticRight(R_angle);
			setMotorRight(R_throttle);
		}
		else if(R_mode == MODE_GROUPS){

			if(R_current_group < R_groups)
			{
				if(R_group_progress > R_group_len - 0.06){
					R_current_group++;
					R_group_progress = 0;
					R_count_zeroer = counter_right;
					R_wait_for_servo = true;
					motor_vel_right = 0;
				}
				else{
					R_group_progress = (counter_right-R_count_zeroer) * 0.06;
				}

				int grouploc = groupSplit(R_current_group, R_groups, R_angle_min, R_angle_max);
				if(motor_vel_right == 0.0){
					updateStaticRight(grouploc);
				}

				if(R_wait_for_servo){
					setMotorRight(0);
					if(fabs(float(grouploc)/100.0 - wiper_state_right) < 0.006){
						R_wait_for_servo = false;
					}
				}else if(R_group_progress > R_group_len - 0.5){
					setMotorRight(R_throttle * 0.15);
				}else if(R_group_progress > R_group_len - 0.2){
					setMotorRight(R_throttle * 0.03);
				}else{
					setMotorRight(R_throttle);
				}
			}
			else{
				R_start = false;
				setings_updated = true;
				setMotorRight(0);
			}
		}
	}
	else
	{
		if(R_mode == MODE_STATIC){
			updateStaticRight(R_angle);
		}
		setMotorRight(0);
	}

	updateSockets();
	checkESTOP();
}
