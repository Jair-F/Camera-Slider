#pragma once
#include <Arduino.h>

#include "button.hpp"
#include "CircularBuffer.inl"
#include "msgAssert.hpp"

class Joystick_Button_3D : public Button
{
private:
	uint8_t xAxisPin, yAxisPin, zAxisPin;
	uint8_t deadZone = 3;
	bool averaging;
	uint8_t numOfAvg;
	CircularBuffer<int16_t> XavgBuff, YavgBuff, ZavgBuff;

	/**
	 * change event handler func pointer for X axis if the value is out of dead zone
	 */
	void (*XChangeEvent)(int16_t axisState) = nullptr;
	/**
	 * change event handler func pointer for X axis if the value is out of dead zone
	 */
	void (*YChangeEvent)(int16_t axisState) = nullptr;
	/**
	 * change event handler func pointer for X axis if the value is out of dead zone
	 */
	void (*ZChangeEvent)(int16_t axisState) = nullptr;

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
					   void (*buttonStateChangeEvent)(bool event, uint32_t eventDuration),
					   void (*_XChangeEvent)(int16_t axisState),
					   void (*_YChangeEvent)(int16_t axisState),
					   void (*_ZChangeEvent)(int16_t axisState),
					   bool _averaging = false, uint8_t _numOfAvg = 5);

	virtual ~Joystick_Button_3D() {}

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
	void begin() override;

	// @return -512 to 512
	inline int16_t readX();
	// @return -512 to 512
	inline int16_t readY();
	// @return -512 to 512
	inline int16_t readZ();

	/**
	 * reads the axes and buttons and calls the callback functions if an event occours
	 */
	void loop() override;
};

Joystick_Button_3D::Joystick_Button_3D(uint8_t _buttonPin, uint8_t _xAxisPin,
									   uint8_t _yAxisPin, uint8_t _zAxisPin,
									   void (*buttonStateChangeEvent)(bool, uint32_t),
									   void (*_XChangeEvent)(int16_t),
									   void (*_YChangeEvent)(int16_t),
									   void (*_ZChangeEvent)(int16_t),
									   bool _averaging, uint8_t _numOfAvg)
	: Button(_buttonPin, buttonStateChangeEvent),
	  xAxisPin(_xAxisPin), yAxisPin(_yAxisPin), zAxisPin(_zAxisPin),
	  averaging(_averaging), numOfAvg(_numOfAvg),
	  XavgBuff(0, numOfAvg), YavgBuff(0, numOfAvg), ZavgBuff(0, numOfAvg),
	  XChangeEvent(_XChangeEvent), YChangeEvent(_YChangeEvent), ZChangeEvent(_ZChangeEvent)
{
	assert(this->XChangeEvent != nullptr, __FILE__, __LINE__);
	assert(this->YChangeEvent != nullptr, __FILE__, __LINE__);
	assert(this->ZChangeEvent != nullptr, __FILE__, __LINE__);
}

void Joystick_Button_3D::begin()
{
	Button::begin();
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
	return retValue;
}

int16_t Joystick_Button_3D::readX()
{
	auto retValue = this->readAxis(this->xAxisPin);

	if (this->averaging)
	{
		this->XavgBuff.put(retValue);
		int16_t avg = 0;
		for (uint16_t i = 0; i < this->XavgBuff.length(); ++i)
		{
			avg += XavgBuff.getValue(i);
		}
		avg /= this->XavgBuff.length();
		return avg;
	}
	else
		return retValue;
}

int16_t Joystick_Button_3D::readY()
{
	auto retValue = this->readAxis(this->yAxisPin);

	if (this->averaging)
	{
		this->YavgBuff.put(retValue);
		int16_t avg = 0;
		for (uint16_t i = 0; i < this->YavgBuff.length(); ++i)
		{
			avg += YavgBuff.getValue(i);
		}
		avg /= this->YavgBuff.length();
		return avg;
	}
	else
		return retValue;
}

int16_t Joystick_Button_3D::readZ()
{
	auto retValue = this->readAxis(this->zAxisPin);

	if (this->averaging)
	{
		this->ZavgBuff.put(retValue);
		int16_t avg = 0;
		for (uint16_t i = 0; i < this->ZavgBuff.length(); ++i)
		{
			avg += ZavgBuff.getValue(i);
		}
		avg /= this->ZavgBuff.length();
		return avg;
	}
	else
		return retValue;
}

void Joystick_Button_3D::loop()
{
	Button::loop();
	if (this->readX() != 0)
		this->XChangeEvent(this->XavgBuff.getValue(0));
	if (this->readY() != 0)
		this->YChangeEvent(this->YavgBuff.getValue(0));
	if (this->readZ() != 0)
		this->ZChangeEvent(this->ZavgBuff.getValue(0));
}
