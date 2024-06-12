#include "driver/ledc.h"

#define PIN_MOTOR1_RIGHT 25
#define PIN_MOTOR2_RIGHT 26

#define PIN_MOTOR1_LEFT 32
#define PIN_MOTOR2_LEFT 33

#define MIN_THROTTLE_LEFT 0.51//0.21//0.7
#define MIN_THROTTLE_RIGHT 0.46//0.18//0.65

#define MAX_THROTTLE 0.92

#define MOTOR_DELTA 0.004
#define MOTOR_DELTA_BRAKE 0.006

extern bool L_dir;
extern bool R_dir;

ledc_channel_config_t ledc_channels[4];

float motor_vel_right = 0.0;
float motor_vel_left = 0.0;

unsigned long update_time_left = millis();
unsigned long update_time_right = millis();

// LEDC_TIMER_1_BIT, 40000000
// LEDC_TIMER_2_BIT, 20000000      
// LEDC_TIMER_3_BIT, 10000000     
// LEDC_TIMER_4_BIT,  5000000    
// LEDC_TIMER_5_BIT,  2500000   
// LEDC_TIMER_6_BIT,  1250000    
// LEDC_TIMER_7_BIT,   625000   
// LEDC_TIMER_8_BIT,   312500  
// LEDC_TIMER_9_BIT,   156250   
// LEDC_TIMER_10_BIT,   78125  
// LEDC_TIMER_11_BIT,   39062  
// LEDC_TIMER_12_BIT,   19531    
// LEDC_TIMER_13_BIT,    9765  
// LEDC_TIMER_14_BIT,    4882 
// LEDC_TIMER_15_BIT,    2441  
// LEDC_TIMER_16_BIT,    1220  
// LEDC_TIMER_17_BIT,     610  
// LEDC_TIMER_18_BIT,     305
// LEDC_TIMER_19_BIT,     152 
// LEDC_TIMER_20_BIT,      76 

void initMotors(){
	pinMode(PIN_MOTOR1_RIGHT, OUTPUT);
	pinMode(PIN_MOTOR2_RIGHT, OUTPUT);
	pinMode(PIN_MOTOR1_LEFT, OUTPUT);
	pinMode(PIN_MOTOR2_LEFT, OUTPUT);

	// configure LEDC timer
	ledc_timer_config_t ledc_timer;
	ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_timer.duty_resolution = LEDC_TIMER_12_BIT;
	ledc_timer.timer_num = LEDC_TIMER_3;
	ledc_timer.freq_hz = 19531;
	ledc_timer_config(&ledc_timer);

	ledc_channels[0].channel = LEDC_CHANNEL_4;
	ledc_channels[0].gpio_num = PIN_MOTOR1_RIGHT;

	ledc_channels[1].channel = LEDC_CHANNEL_5;
	ledc_channels[1].gpio_num = PIN_MOTOR2_RIGHT;

	ledc_channels[2].channel = LEDC_CHANNEL_6;
	ledc_channels[2].gpio_num = PIN_MOTOR1_LEFT;

	ledc_channels[3].channel = LEDC_CHANNEL_7;
	ledc_channels[3].gpio_num = PIN_MOTOR2_LEFT;

	for (int i = 0; i < 4; i++) {
		ledc_channels[i].timer_sel = LEDC_TIMER_3;
		ledc_channels[i].duty = 0;
		ledc_channel_config(&ledc_channels[i]);
	}
}

int fix_speed(int mot_speed, float min_throttle){
	if(mot_speed == 0)
		return 0;

	if(mot_speed > 0){
		float offset = min_throttle*100.0 + mot_speed*(1.0 - min_throttle - (1.0 - MAX_THROTTLE));
		if(offset > 100)
			return 100;
		return (int)offset;
	}

	if(mot_speed < 0){
		float offset = -min_throttle*100.0 + mot_speed*(1.0 - min_throttle - (1.0 - MAX_THROTTLE));
		if(offset < -100)
			return -100;
		return (int)offset;	
	}
}

void setPwm(int id, float vel){

	if(vel > MAX_THROTTLE-0.01)
		vel = 1.0;
	else if(vel < -MAX_THROTTLE+0.01)
		vel = -1.0;
	
	ledc_set_duty(LEDC_HIGH_SPEED_MODE, ledc_channels[id].channel, int(abs(vel*4095)));
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, ledc_channels[id].channel);
}

void setMotorLeft(int mot_speed){

	if(millis() - update_time_left < 40){
		return;
	}

	if(L_dir){
		mot_speed = -mot_speed;
	}

	mot_speed = fix_speed(mot_speed, MIN_THROTTLE_LEFT);

	if(mot_speed > 0){
		motor_vel_left = motor_vel_left += MOTOR_DELTA;

		if(motor_vel_left > mot_speed/100.0){
			motor_vel_left = mot_speed/100.0;
		}		
	}else{
		motor_vel_left = motor_vel_left -= MOTOR_DELTA_BRAKE;

		if(motor_vel_left < mot_speed/100.0){
			motor_vel_left = mot_speed/100.0;
		}
	}

	if(motor_vel_left > -MIN_THROTTLE_LEFT && motor_vel_left < MIN_THROTTLE_LEFT){
		if(mot_speed > 0){
			motor_vel_left = MIN_THROTTLE_LEFT;
		}
		else if(mot_speed < 0){
			motor_vel_left = -MIN_THROTTLE_LEFT;
		}
		else{
			motor_vel_left = 0;
		}
	}
	
	if(motor_vel_left < 0){
		setPwm(0,motor_vel_left);
		setPwm(1,0);
	}else{
		setPwm(0,0);
		setPwm(1,motor_vel_left);
	}
	

	update_time_left = millis();
}

void setMotorRight(int mot_speed){

	if(millis() - update_time_right < 40){
		return;
	}

	if(R_dir){
		mot_speed = -mot_speed;
	}

	mot_speed = fix_speed(mot_speed, MIN_THROTTLE_RIGHT);

	if(mot_speed > 0){
		motor_vel_right = motor_vel_right += MOTOR_DELTA;

		if(motor_vel_right > mot_speed/100.0){
			motor_vel_right = mot_speed/100.0;
		}		
	}else{
		motor_vel_right = motor_vel_right -= MOTOR_DELTA;

		if(motor_vel_right < mot_speed/100.0){
			motor_vel_right = mot_speed/100.0;
		}
	}

	if(motor_vel_right > -MIN_THROTTLE_RIGHT && motor_vel_right < MIN_THROTTLE_RIGHT){
		if(mot_speed > 0){
			motor_vel_right = MIN_THROTTLE_RIGHT;
		}
		else if(mot_speed < 0){
			motor_vel_right = -MIN_THROTTLE_RIGHT;
		}
		else{
			motor_vel_right = 0;
		}
	}

	if(motor_vel_right < 0){
		setPwm(2,0);
		setPwm(3,motor_vel_right);
	}else{
		setPwm(2,motor_vel_right);
		setPwm(3,0);
	}

	update_time_right = millis();
}
