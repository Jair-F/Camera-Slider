#include <Arduino.h>

// Joystick-Pins
#define VRX A0
#define VRY A1
#define SW  2


class JoyStick {
public:
	/*
		@param _VRX_pin has to be an analogPin
		@param _VRY_pin has to be an analogPin
	*/
	JoyStick(uint8_t _VRX_pin, uint8_t _VRY_pin, uint8_t _SW_pin);
	JoyStick() { }

	// @return a number between 0 - 1023
	unsigned short x_axis()		{	return analogRead(VRX_pin);	}
	// @return a number between 0 - 1023
	unsigned short y_axis()		{	return analogRead(VRY_pin);	}
	// @return true if the button is pressed
	bool sw_button()	{	return !digitalRead(SW_pin);	}

	void set_VRX_pin(uint8_t _VRX_pin)	{	VRX_pin = _VRX_pin; pinMode(VRX_pin, INPUT);	}
	void set_VRY_pin(uint8_t _VRY_pin)	{	VRY_pin = _VRY_pin; pinMode(VRY_pin, INPUT);	}
	void set_SW_pin(uint8_t _SW_pin)	{	SW_pin = _SW_pin; pinMode(SW_pin, INPUT);		}
	
private:
	uint8_t VRX_pin, VRY_pin, SW_pin;
};


JoyStick::JoyStick(uint8_t _VRX_pin, uint8_t _VRY_pin, uint8_t _SW_pin): VRX_pin(_VRX_pin), VRY_pin(_VRY_pin), SW_pin(_SW_pin) {
	pinMode(VRX_pin, INPUT);
	pinMode(VRY_pin, INPUT);
	pinMode(SW_pin, INPUT_PULLUP);		// The button is a pullup
}
