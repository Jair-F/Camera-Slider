#include "StepMotor.hpp"

#pragma once

#pragma once
/*
struct Position {
	long x, y, z;
};
*/
class CameraSlider {
public:
	enum Axis {
		X_AXIS, Y_AXIS, Z_AXIS
	};

	CameraSlider(int number_of_steps_in_one_revolution, uint8_t x_axis_pin_1, uint8_t x_axis_pin_2, uint8_t x_axis_pin_3, uint8_t x_axis_pin_4, 
				 uint8_t y_axis_pin_1, uint8_t y_axis_pin_2, uint8_t y_axis_pin_3, uint8_t y_axis_pin_4, 
				 uint8_t z_axis_pin_1, uint8_t z_axis_pin_2, uint8_t z_axis_pin_3, uint8_t z_axis_pin_4):	x_axis(number_of_steps_in_one_revolution, x_axis_pin_1, x_axis_pin_2, x_axis_pin_3, x_axis_pin_4),
				 																							y_axis(number_of_steps_in_one_revolution, y_axis_pin_1, y_axis_pin_2, y_axis_pin_3, y_axis_pin_4),
				 																							z_axis(number_of_steps_in_one_revolution, z_axis_pin_1, z_axis_pin_2, z_axis_pin_3, z_axis_pin_4) { }

	Position getCurrentPosition();

	// @return true if the motor is still running to a point.
	bool run();

	/*
		The speed with acceleration/deacceleration. this is the speed for the longest distanceToGo-axis. The other axises
		get their speed accroding to their path to run to ensure they all arrive at the endpoint at the same time
	*/
	void setSpeed(double _speed);
	// Max speed, which will not be exceeded. Every time the speed is set it will be checked. If it exceeds, it will throw an exception
	void setMaxSpeed(double _maxSpeed);

	/*
		param lastStepSpeed the speed, the motor will run on the last step(the motor will incerase or decerase his speed constantly
			  until he reaches the last step and will then run the last step with this speed - smooth acceleration/deacceleration)
	*/
	void moveTo(Position pos, double lastStepSpeed);

	void forceStop();

	/*
		@param _axis The axis, the distanceTraveld should be taken
		@return get the distanceTraveld from the given axis
	*/
	long long distanceTraveld(Axis _axis);

	/*
		set distanceTraveld to all axex
	*/
	void setDistanceTraveld(long long _distanceTraveld = 0);

protected:
	/*
		Calculates the speed for every axis according to their distance they have to go, that all the axis need the same time to travel different lenth of wayss
	*/
	void calcAndSetNewSpeed(double speed);

private:
	StepMotor x_axis;
	StepMotor y_axis;
	StepMotor z_axis;

	double speed;
	double maxSpeed;
};



// Implementation

void CameraSlider::setDistanceTraveld(long long _distanceTraveld) {
	x_axis.setDistanceTraveld(_distanceTraveld);
	y_axis.setDistanceTraveld(_distanceTraveld);
	z_axis.setDistanceTraveld(_distanceTraveld);
}

long long CameraSlider::distanceTraveld(CameraSlider::Axis _axis) {
	switch (_axis) {
		case CameraSlider::Axis::X_AXIS:
			return x_axis.distanceTraveld();

		case CameraSlider::Axis::Y_AXIS:
			return y_axis.distanceTraveld();

		case CameraSlider::Axis::Z_AXIS:
			return z_axis.distanceTraveld();
		
		default:
			break;
	}
}

void CameraSlider::forceStop() {
	x_axis.forceStop();
	y_axis.forceStop();
	z_axis.forceStop();
}

void CameraSlider::moveTo(Position pos, double lastStepSpeed) {
	x_axis.moveTo(pos.x, lastStepSpeed);
	y_axis.moveTo(pos.y, lastStepSpeed);
	z_axis.moveTo(pos.z, lastStepSpeed);
	this->calcAndSetNewSpeed(speed);
}

void CameraSlider::setSpeed(double _speed) {
	if(_speed <= maxSpeed) {
		speed = _speed;
		this->calcAndSetNewSpeed(speed);
	}
	else {
		// Need to implement other exception
		Serial.println("Error - speed > maxSpeed");
	}
}

void CameraSlider::setMaxSpeed(double _maxSpeed) {
	x_axis.setMaxSpeed(maxSpeed);
	y_axis.setMaxSpeed(maxSpeed);
	z_axis.setMaxSpeed(maxSpeed);
	maxSpeed = maxSpeed;
}

Position CameraSlider::getCurrentPosition() {
	Position ret;
	ret.x = x_axis.currentPosition();
	ret.y = y_axis.currentPosition();
	ret.z = z_axis.currentPosition();
	return ret;
}

void CameraSlider::calcAndSetNewSpeed(double speed) {
	double one_precent_of_speed = speed / 100;
	/*
		find greatest distance, because we need to set the speed accordingly(if we have a shorter way to go we need to reduce the speed)
		The greatest distance has 100% of the speed. The others are changed to the precent of their range(greatest_distance * 100 / other_distance)
	*/
	long greatest_distance = x_axis.distanceToGo();
	if(y_axis.distanceToGo() > greatest_distance) {
		greatest_distance = y_axis.distanceToGo();
	} 
	else if (y_axis.distanceToGo() > x_axis.distanceToGo()) {
		greatest_distance = y_axis.distanceToGo();
	}

	double one_percent_of_greatest_distance = greatest_distance / 100.0;	// Convert the 100 to a float(100.0) in order to not cut the numbers after the comma

	x_axis.setMaxSpeed(x_axis.distanceToGo() / one_percent_of_greatest_distance * one_precent_of_speed);
	y_axis.setMaxSpeed(y_axis.distanceToGo() / one_percent_of_greatest_distance * one_precent_of_speed);
	z_axis.setMaxSpeed(z_axis.distanceToGo() / one_percent_of_greatest_distance * one_precent_of_speed);
}

bool CameraSlider::run() {
	return x_axis.run() || y_axis.run() || z_axis.run();
}
