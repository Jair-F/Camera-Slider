#include <Stepper.h>
#include <math.h>

#pragma once

class StepMotor {
public:
	StepMotor(int number_of_steps_in_one_revolution, int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4, double _maxSpeed = 10): motor(number_of_steps_in_one_revolution, motor_pin_1, motor_pin_2, motor_pin_3, motor_pin_4), maxSpeed(_maxSpeed) {
		motor.setSpeed(maxSpeed);
	}

	long long currentPosition() { return current_position; }

	void setCurrentPosition(long long _pos) { current_position = _pos; }

	void setSpeedChangePerStep(double _speedChangePerStep) { speed_change_per_step = _speedChangePerStep; }

	/*
		@param _maxSpeed in this case the higher the value is the faster the motor will move(figure out the value for your motor)
	*/
	void setMaxSpeed(double _maxSpeed) { maxSpeed = _maxSpeed; }
	
	double currenSpeed() { return current_speed; }
	double speedChangePerStep() { return speed_change_per_step; }

	/*
		@param pos target-Position the motor will move to(positive is clockwise, negative counter clockwise)
		@param _lastStepSpeed The motor will incerase his speed from the previous set speed to this speed(_lastStepSpeed) in such a way
		that he incerases the speed constantly at every step and this value is the speed of the las step
	*/
	void moveTo(long long pos);

	/*
		Goes only one step forward with the given speed
		@param direction true for clockwise
		@param speed the delay in milliseconds between the steps
	*/
	void move(bool direction, double speed);

	/*
		@return distance from the actual position to the target Position. positive if we move clockwise
	*/
	long long distanceToGoToTargetPosition() { return target_position - current_position; }

	/*
		Need to be called in the main loop. Makes a step if a step is due.
		@return true if the motor made at this call of this run-function a step
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

private:
	Stepper motor;

	double maxSpeed;

	long long current_position;	// the position(if we are on position 0 and go then 10 to plus and afterwards 10 to minus the position os again 0)
	long long target_position;

	double current_speed;				// The actual Speed. speed is the of milliseconds number, we delay between the single steps!
	double speed_change_per_step;	// In order to reach the target_last_step_speed we need to incerase the speed of each step with this value

	long long last_step_time_point = 0;	// The time point, we made the last step to move to the target-Position
};

// Implementations

void StepMotor::moveTo(long long pos) {
	target_position = pos;
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
	
	current_speed = speed;

	long difference = timer_end - timer_begin;
	if(speed - difference > 0) {
		delay(speed - difference);	// slepp if needed to set the right speed
	}
}

bool StepMotor::run() {
	// Step only one step if the speed=(time between the steps) is passed and we are not on the targetPosition
	bool step_is_due = millis() >= last_step_time_point + current_speed && current_position != target_position;
	if(step_is_due) {
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
	return step_is_due;
}

void StepMotor::forceStop() {
	target_position = current_position;
}
