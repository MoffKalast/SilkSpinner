#define PIN_HALL_RIGHT 34
#define PIN_HALL_LEFT 35

#define SIG_HIGH 1700
#define SIG_LOW 1200

#define DEBOUNCE_MS 140

int counter_left = 0;
int counter_right = 0;

float frequency_right = 0;
float frequency_left = 0;

unsigned long prev_spin_time_left = millis();
unsigned long spin_time_left = millis();

unsigned long prev_spin_time_right = millis();
unsigned long spin_time_right = millis();

int prev_counter_left = 0;
int prev_counter_right = 0;

unsigned long update_time_hall = millis();

int state_left = 0;
int state_right = 0;

int getMagnetState(int rawmagnet){
	if(rawmagnet < SIG_LOW)
		return 0;
	if(rawmagnet > SIG_HIGH)
		return 2;
	return 1;
}

// val 0 low, 1 mid, 2 high
int stateMachine(int state, int rawmagnet){

	if(state > 9)
		return state;
	
    const int transitions[][2] = {
        {1, 2},
        {2, 2},
        {3, 2},
        {4, 2},
        {5, 2},
        {6, 1},
        {7, 1},
        {8, 0},
        {9, 0},
        {10, 0}
    };

    int match = transitions[state][1];

    if(getMagnetState(rawmagnet) == match)	
    	return transitions[state][0];
    	
    return state;
}

void readSensors(){
	if(millis() - update_time_hall < 2){
		return;
	}

	state_left = stateMachine(state_left, analogRead(PIN_HALL_LEFT));
	if(state_left == 10 ){

		if(millis() - spin_time_left > DEBOUNCE_MS){
			counter_left++;

			prev_spin_time_left = spin_time_left;
			spin_time_left = millis();	

			frequency_left = 333.3333/float(spin_time_left - prev_spin_time_left);
		}
		
		state_left = 0;
	}

	state_right = stateMachine(state_right, analogRead(PIN_HALL_RIGHT));
	if(state_right == 10){

		if(millis() - spin_time_right > DEBOUNCE_MS){
			counter_right++;
	
			prev_spin_time_right = spin_time_right;
			spin_time_right = millis();

			frequency_right = 333.3333/float(spin_time_right - prev_spin_time_right);
		}
		
		state_right = 0;
	}

	update_time_hall = millis();
}
