#pragma once
#include <Arduino.h>
#include "msgAssert.hpp"

class Button
{
private:
	uint8_t pin;
	bool lastState = false;
	uint32_t stateChangeTmPoint = 0;

	/**
	 * @param event the current state of the button. true for clicked
	 * @param eventDuration duration since the last event change.
	 * 	if event is clicked then its since the last click.
	 * 	if event isnt clicked then its since last release of the button.
	 */
	void (*stateChangeEvent)(bool event, uint32_t eventDuration) = nullptr;

public:
	Button(uint8_t _pin, void (*stateChangeEvent)(bool event, uint32_t eventDuration));
	virtual ~Button() {}

	virtual void begin();
	inline bool getButtonState() const { return digitalRead(this->pin); }
	// @return last read state of button in loop
	inline bool getLastButtonState() const { return this->lastState; }
	virtual void loop();
};

Button::Button(uint8_t _pin,
			   void (*_stateChangeEvent)(bool event, uint32_t eventDuration)) : pin(_pin),
																				stateChangeEvent(_stateChangeEvent)
{
	assert(this->stateChangeEvent != nullptr, __FILE__, __LINE__);
}

void Button::begin()
{
	pinMode(this->pin, OUTPUT);
}

void Button::loop()
{
	bool buttonState = digitalRead(this->pin);
	if (this->lastState != buttonState)
	{
		uint16_t stateChangeDuration = millis() - this->stateChangeTmPoint;
		this->stateChangeTmPoint = millis();

		this->stateChangeEvent(buttonState, stateChangeDuration); // call the event func

		this->lastState = buttonState;
	}
}
