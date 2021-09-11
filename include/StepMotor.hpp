#include <Stepper.h>

/*
AccelStepper motor(64, 8, 10, 9, 11);

#define STEPS_PER_REVOLUTION 2048
*/

#pragma once

#define STRIDE_ANGLE 5.625

#define GEAR_REDUCTION 32

#define STEPS_PER_REVOLUTION _ 2048

// Revolutions per minute(rpm)
#define MAX_SPEED 15

class StepMotor {
public:
	StepMotor(int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4, int motor_pin_5, unsigned int steps_per_revolution = 64): motor(steps_per_revolution, motor_pin_1, motor_pin_2, motor_pin_3, motor_pin_4) { }
	
	void setCurrentPosition(long long pos) { current_position = pos; }
	long long currentPositon() { return current_position; }

	void move(long long absolute_position);
	void moveTo(long long relative_position);

	void setMaxSpeed();
private:
	Stepper motor;
	long long current_position;
};
