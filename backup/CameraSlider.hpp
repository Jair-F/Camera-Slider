#include <AccelStepper.h>

#pragma once
/*
struct Position {
	long x, y, z;
};
*/
class CameraSlider {
public:
	CameraSlider(uint8_t x_axis_pin_1, uint8_t x_axis_pin_2, uint8_t x_axis_pin_3, uint8_t x_axis_pin_4, 
				 uint8_t y_axis_pin_1, uint8_t y_axis_pin_2, uint8_t y_axis_pin_3, uint8_t y_axis_pin_4, 
				 uint8_t z_axis_pin_1, uint8_t z_axis_pin_2, uint8_t z_axis_pin_3, uint8_t z_axis_pin_4):	x_axis(AccelStepper::FULL4WIRE, x_axis_pin_1, x_axis_pin_2, x_axis_pin_3, x_axis_pin_4),
				 																							y_axis(AccelStepper::FULL4WIRE, y_axis_pin_1, y_axis_pin_2, y_axis_pin_3, y_axis_pin_4),
				 																							z_axis(AccelStepper::FULL4WIRE, z_axis_pin_1, z_axis_pin_2, z_axis_pin_3, z_axis_pin_4) { }

	Position getCurrentPosition();

	void run();
	// run to the target point which was set previously with the given speed
	void runSpeed(float _speed);

	/*
		The speed with acceleration/deacceleration. this is the speed for the longest distanceToGo-axis. The other axises
		get their speed accroding to their path to run to ensure they all arrive at the endpoint at the same time
	*/
	void setSpeed(float _speed);
	// Max speed, which will not be exceeded. Every time the speed is set it will be checked. If it exceeds, it will throw an exception
	void setMaxSpeed(float _maxSpeed);

	void moveTo(Position pos);

	void setAcceleration(float acceleration);

protected:
	/*
		Calculates the speed for every axis according to their distance they have to go, that all the axis need the same time to travel different lenth of wayss
	*/
	void calcAndSetNewSpeed(float speed);

private:
	AccelStepper x_axis;
	AccelStepper y_axis;
	AccelStepper z_axis;

	float speed;
	float maxSpeed;
};



// Implementation

void CameraSlider::moveTo(Position pos) {
	x_axis.moveTo(pos.x);
	y_axis.moveTo(pos.y);
	z_axis.moveTo(pos.z);
	this->calcAndSetNewSpeed(speed);
}

void CameraSlider::setSpeed(float _speed) {
	if(_speed <= maxSpeed) {
		speed = _speed;
		this->calcAndSetNewSpeed(speed);
	}
	else {
		// Need to implement other exception
		Serial.println("Error - speed > maxSpeed");
	}
}

void CameraSlider::setMaxSpeed(float _maxSpeed) {
	x_axis.setMaxSpeed(maxSpeed);
	y_axis.setMaxSpeed(maxSpeed);
	z_axis.setMaxSpeed(maxSpeed);
	maxSpeed = maxSpeed;
}

void CameraSlider::setAcceleration(float acceleration) {
	x_axis.setAcceleration(acceleration);
	y_axis.setAcceleration(acceleration);
	z_axis.setAcceleration(acceleration);
}

Position CameraSlider::getCurrentPosition() {
	Position ret;
	ret.x = x_axis.currentPosition();
	ret.y = y_axis.currentPosition();
	ret.z = z_axis.currentPosition();
	return ret;
}

void CameraSlider::calcAndSetNewSpeed(float speed) {
	float one_precent_of_speed = speed / 100;
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

	float one_percent_of_greatest_distance = greatest_distance / 100.0;	// Convert the 100 to a float(100.0) in order to not cut the numbers after the comma

	x_axis.setMaxSpeed(x_axis.distanceToGo() / one_percent_of_greatest_distance * one_precent_of_speed);
	y_axis.setMaxSpeed(y_axis.distanceToGo() / one_percent_of_greatest_distance * one_precent_of_speed);
	z_axis.setMaxSpeed(z_axis.distanceToGo() / one_percent_of_greatest_distance * one_precent_of_speed);
}

void CameraSlider::run() {
	x_axis.run();
	y_axis.run();
	z_axis.run();
}

void CameraSlider::runSpeed(float _speed) {
	if(_speed <= maxSpeed) {
		x_axis.setSpeed(_speed);
		y_axis.setSpeed(_speed);
		z_axis.setSpeed(_speed);

		x_axis.runSpeed();
		y_axis.runSpeed();
		z_axis.runSpeed();
	}
	else {
		Serial.println("Error - speed > maxSpeed");
	}
}
