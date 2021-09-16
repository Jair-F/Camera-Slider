#include <Stepper.h>
#include <math.h>

struct Position {
	long x, y, z;
};

struct SpeedChange {
	unsigned int speed;
	Position speedChangePosition;
};


Position positions[10];
SpeedChange speeds[5];


class StepMotor {
public:
	StepMotor(int number_of_steps_in_one_revolution, int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4, long _maxSpeed = 10): motor(number_of_steps_in_one_revolution, motor_pin_1, motor_pin_2, motor_pin_3, motor_pin_4), maxSpeed(_maxSpeed) {
		motor.setSpeed(maxSpeed);
	}

	long long currentPosition() { return current_position; }

	void setCurrentPosition(long long _pos) { current_position = _pos; }

	/*
		@param _maxSpeed in this case the higher the value is the faster the motor will move(figure out the value for your motor)
	*/
	void setMaxSpeed(long _maxSpeed) { maxSpeed = _maxSpeed; }
	
	double getCurrenSpeed() { return current_speed; }
	double getCurrenSpeedChange() { return speed_change_per_step; }

	/*
		@param pos target-Position the motor will move to(positive is clockwise, negative counter clockwise)
		@param _lastStepSpeed The motor will incerase his speed from the previous set speed to this speed(_lastStepSpeed) in such a way
		that he incerases the speed constantly at every step and this value is the speed of the las step
	*/
	void moveTo(long long pos, long double _lastStepSpeed);

	/*
		Goes only one step forward with the given speed
		@param direction true for clockwise
	*/
	void move(bool direction, double speed);

	/*
		@return distance from the actual position to the target Position. positive if we move clockwise
	*/
	long long distanceToGo() { return target_position - current_position; }

	/*
		Need to be called in the main loop. Makes a step if a step is due.
		@return true if the motor is still running to a point.
	*/
	bool run();

	/*
		@return true if the stepper is not on the targetPosition(actually moving)
	*/
	bool isRunning() { return current_position != target_position; }

	/*
		force stops the motor(sets the targetPosition to the currentPosition)
	*/
	void forceStop();

protected:
	/*
		Calcs the new speed_incerase_per_step for the actual set target_position, target_last_step_speed and actual_speed
		The metiont parameters need to be already set to the new parameters
	*/
	void calc_and_set_new_speeds() {
		speed_change_per_step = (abs(target_last_step_speed - current_speed)) / (abs(target_position - current_position));

		// If we need to decerase the speed
		if(target_last_step_speed < current_speed) {
			speed_change_per_step = -speed_change_per_step;
		}
	}

private:
	Stepper motor;

	long maxSpeed;

	long long current_position;
	long long target_position;
	
	double current_speed;				// The actual Speed. speed is the of milliseconds number, we delay between the single steps!
	double target_last_step_speed;	// The speed of the last step should be this speed
	double speed_change_per_step;	// In order to reach the target_last_step_speed we need to incerase the speed of each step with this value

	long long last_step_time_point = 0;
};

void StepMotor::moveTo(long long pos, long double _lastStepSpeed) {
	target_position = pos;
	target_last_step_speed = _lastStepSpeed;

	calc_and_set_new_speeds();
}

void StepMotor::move(bool direction, double speed) {
	this->forceStop();

	long timer_begin = millis();
	if(direction == true) {
		motor.step(1);
		++current_position;
	}
	else {
		motor.step(-1);
		--current_position;
	}
	long timer_end = millis();

	long difference = timer_end - timer_begin;
	if(speed - difference > 0) {
		delay(speed - difference);	// slepp if needed to set the right speed
	}
}

bool StepMotor::run() {
	// Step only one step if the speed=(time between the steps) is passed and we are not on the targetPosition
	if(millis() >= last_step_time_point + current_speed && current_position != target_position) {
		last_step_time_point = millis();

		current_speed += speed_change_per_step;
		
		if(target_position < current_position) {
			motor.step(-1);
			current_position -= 1;
		} else {
			motor.step(1);
			current_position += 1;
		}
	}
	return isRunning();
}

void StepMotor::forceStop() {
	target_position = current_position;
}
