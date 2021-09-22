#include "StepMotor.hpp"

#pragma once

#pragma once
/*
struct Position {
	long x, y, z;
};
*/

struct Position {
	long x, y, z;
	// If the position is activated(valid, parameters set)
	bool activated;
};

struct SpeedChange {
	double speed;
	/*
		The point, where the speed is reached - from the previous point and speed
		In reference to the distanceTraveld of the reference_axis
	*/
	long long speedChangePosition;

	unsigned short speedChangePositionInPercent;

	CameraSlider::Axis reference_axis;
};


class CameraSlider {
public:
	enum Axis {
		X_AXIS, Y_AXIS, Z_AXIS
	};

	CameraSlider(int number_of_steps_in_one_revolution,
				 uint8_t x_axis_pin_1, uint8_t x_axis_pin_2, uint8_t x_axis_pin_3, uint8_t x_axis_pin_4, 
				 uint8_t y_axis_pin_1, uint8_t y_axis_pin_2, uint8_t y_axis_pin_3, uint8_t y_axis_pin_4, 
				 uint8_t z_axis_pin_1, uint8_t z_axis_pin_2, uint8_t z_axis_pin_3, uint8_t z_axis_pin_4):	x_axis(number_of_steps_in_one_revolution, x_axis_pin_1, x_axis_pin_2, x_axis_pin_3, x_axis_pin_4),
				 																							y_axis(number_of_steps_in_one_revolution, y_axis_pin_1, y_axis_pin_2, y_axis_pin_3, y_axis_pin_4),
				 																							z_axis(number_of_steps_in_one_revolution, z_axis_pin_1, z_axis_pin_2, z_axis_pin_3, z_axis_pin_4) { }

	Position currentPosition();

	// noch ueberdenken ob das sinn macht
	double currentSpeed();

	/*
		Need to be called in the main loop. Let the motors make a step, if a step is due(if we are moving to a target position). 
		@return true if the motor is still running to a point.
	*/
	bool run();

	/*
		The speed with acceleration/deacceleration. this is the speed for the longest distanceToGoToTargetDistanceTraveld-axis. 
		The other axes get their speed accroding to their distanceToTravel to ensure they all arrive at the endpoint at the same time. 
		This function sets the last step speed and calculate from it the new acceleration to the position of the traveld distance
		@param lastStepSpeed the speed, the motor will turn on the last step to the traveldDistancePosition
		@param traveldDistancePosition The speed will de-/accelerate until the motor reaches this point of made steps(doesnt depends in which direction the motor turned)
	*/
	void setLastStepSpeed(SpeedChange _speed);
	// Max speed, which will not be exceeded. Every time the speed is set it will be checked. If it exceeds, it will throw an exception
	void setMaxSpeed(double _maxSpeed);

	/*
		param lastStepSpeed the speed, the motor will run on the last step(the motor will incerase or decerase his speed constantly
			  until he reaches the last step and will then run the last step with this speed - smooth acceleration/deacceleration)
	*/
	void moveTo(Position pos);

	void forceStop();

	/*
		@return get the distanceTraveld from the given axis
	*/
	long long distanceTraveld();

	/*
		set distanceTraveld
	*/
	void setDistanceTraveld(long long _distanceTraveld = 0);

protected:
	/*
		Calculates the speed for every axis according to their distance they have to go, that all the axis need the same time to travel different lenth of wayss
	*/
	void calcAndSetNewSpeed(SpeedChange _speed);

private:
	StepMotor x_axis;
	StepMotor y_axis;
	StepMotor z_axis;

	SpeedChange lastStepSpeed;

	Axis reference_axis;
	long long distance_traveld;

	double speed;
	double maxSpeed;
};



// Implementation

void CameraSlider::setDistanceTraveld(long long _distanceTraveld) {
	distance_traveld = _distanceTraveld;
}

long long CameraSlider::distanceTraveld() {
	return this->distance_traveld;
}

void CameraSlider::forceStop() {
	x_axis.forceStop();
	y_axis.forceStop();
	z_axis.forceStop();
}

void CameraSlider::moveTo(Position pos) {
	x_axis.moveTo(pos.x);
	y_axis.moveTo(pos.y);
	z_axis.moveTo(pos.z);
	calcAndSetNewSpeed(this->lastStepSpeed);
}

void CameraSlider::setLastStepSpeed(SpeedChange _speed) {
	if(_speed.speed > maxSpeed) {	// throttle down the speed if it exceeds the maxSpeed
		_speed.speed = maxSpeed;
	}
	lastStepSpeed = _speed;
	this->calcAndSetNewSpeed(lastStepSpeed);
}

void CameraSlider::setMaxSpeed(double _maxSpeed) {
	x_axis.setMaxSpeed(maxSpeed);
	y_axis.setMaxSpeed(maxSpeed);
	z_axis.setMaxSpeed(maxSpeed);
	maxSpeed = maxSpeed;
}

Position CameraSlider::currentPosition() {
	Position ret;
	ret.x = x_axis.currentPosition();
	ret.y = y_axis.currentPosition();
	ret.z = z_axis.currentPosition();
	return ret;
}

void CameraSlider::calcAndSetNewSpeed(SpeedChange _speed) {
	double one_precent_of_speed = _speed.speed / 100.0;

	//double one_percent_of_distance_traveld = referenc_axis_distance_traveld / 100.0;	// Convert the 100 to a float(100.0) in order to not cut the numbers after the comma

	double one_percent_of_distance_traveld = this->distance_traveld / 100.0;

	x_axis.setSpeedChangePerStep(x_axis.distanceToGoToTargetPosition() / one_percent_of_distance_traveld * one_precent_of_speed);
	y_axis.setSpeedChangePerStep(y_axis.distanceToGoToTargetPosition() / one_percent_of_distance_traveld * one_precent_of_speed);
	z_axis.setSpeedChangePerStep(z_axis.distanceToGoToTargetPosition() / one_percent_of_distance_traveld * one_precent_of_speed);

	//x_axis.setLastStepSpeed(x_axis.distanceToGoToTargetPosition() / one_percent_of_distance_traveld * one_precent_of_speed);
	//y_axis.setLastStepSpeed(y_axis.distanceToGoToTargetPosition() / one_percent_of_distance_traveld * one_precent_of_speed);
	//z_axis.setLastStepSpeed(z_axis.distanceToGoToTargetPosition() / one_percent_of_distance_traveld * one_precent_of_speed);
}

bool CameraSlider::run() {
	
	switch (this->reference_axis) {
		case Axis::X_AXIS: {
			if(x_axis.run()) {	// If we made a step
				++this->distance_traveld;
			}
			y_axis.run();
			z_axis.run();
			break;
		}
		case Axis::Y_AXIS: {
			if(y_axis.run()) {	// If we made a step
				++this->distance_traveld;
			}
			x_axis.run();
			z_axis.run();
			break;
		}
		case Axis::Z_AXIS: {
			if(z_axis.run()) {
				++this->distance_traveld;
			}
			x_axis.run();
			y_axis.run();
			break;
		}
		default:
			break;
	}

	// If we at the point, where we need to change the speed
	if(distance_traveld >= lastStepSpeed.speedChangePosition) {
		x_axis.setSpeedChangePerStep(0.0);
		y_axis.setSpeedChangePerStep(0.0);
		z_axis.setSpeedChangePerStep(0.0);
	}

	//return x_axis.run() || y_axis.run() || z_axis.run();
	return x_axis.isRunning() || y_axis.isRunning() || z_axis.isRunning();
}
