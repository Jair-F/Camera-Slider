#pragma once
#include <Arduino.h>
#include "CircularBuffer.inl"

class Joystick_Button_3D
{
private:
	uint8_t buttonPin, xAxisPin, yAxisPin, zAxisPin;
	uint8_t deadZone = 3;
	bool averaging;
	uint8_t numOfAvg;
	CircularBuffer<int16_t> avgBuff;

protected:
	/**
	 * reads the given axis and applys dead zone
	 */
	uint16_t readAxis(uint8_t _pin);

public:
	/**
	 * @param _buttonPin digital or analog
	 * @param _xAxisPin analog input
	 * @param _yAxisPin analog input
	 * @param _zAxisPin analog input
	 * @param _averaging true to make average of reads to reduce noise
	 * @param _numOfAvg last num of reads to consider in average
	 */
	Joystick_Button_3D(uint8_t _buttonPin,
					   uint8_t _xAxisPin, uint8_t _yAxisPin, uint8_t _zAxisPin,
					   bool _averaging = false, uint8_t _numOfAvg = 5);

	/**
	 * the range of the axis is -512 to 512. The deadzone sets
	 * the range in + and - to be counted as 0
	 * @param _deadZone
	 */
	inline void setDeadZone(uint8_t _deadZone) { this->deadZone = _deadZone; }
	const uint8_t getDeadZone() const { return this->deadZone; }

	/**
	 * define all outputs on the arduino board
	 */
	void begin();

	// @return -512 to 512
	inline int16_t readX() { return this->readAxis(this->xAxisPin); }
	// @return -512 to 512
	inline int16_t readY() { return this->readAxis(this->yAxisPin); }
	// @return -512 to 512
	inline int16_t readZ() { return this->readAxis(this->zAxisPin); }
};

Joystick_Button_3D::Joystick_Button_3D(uint8_t _buttonPin, uint8_t _xAxisPin,
									   uint8_t _yAxisPin, uint8_t _zAxisPin,
									   bool _averaging, uint8_t _numOfAvg)
	: buttonPin(_buttonPin), xAxisPin(_xAxisPin), yAxisPin(_yAxisPin), zAxisPin(_zAxisPin),
	  averaging(_averaging), numOfAvg(_numOfAvg), avgBuff(0, numOfAvg)
{
}

void Joystick_Button_3D::begin()
{
	pinMode(this->buttonPin, INPUT);
	pinMode(this->xAxisPin, INPUT);
	pinMode(this->yAxisPin, INPUT);
	pinMode(this->zAxisPin, INPUT);
}

uint16_t Joystick_Button_3D::readAxis(uint8_t _pin)
{
	int16_t retValue = analogRead(_pin) - 512;

	// in range of deadZone?
	if (retValue < this->deadZone || retValue > -this->deadZone)
		retValue = 0;

	if (this->averaging)
	{
		this->avgBuff.put(retValue);
		int16_t avg = 0;
		for (uint16_t i = 0; i < this->avgBuff.length(); ++i)
		{
			avg += avgBuff.getValue(i);
		}
		avg /= this->avgBuff.length();
		return avg;
	}
	else
		return retValue;
}
