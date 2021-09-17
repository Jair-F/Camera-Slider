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

	// @return a number between -512 to 512
	short x_axis();
	// @return a number between -512 to 512
	short y_axis();
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

short JoyStick::x_axis() {
	short x_state = analogRead(VRX_pin); // Value between 0 and 1023
	
	// the value x_state can be 0-1023(1024 states)
	return x_state - 1023/2;	// We will return a value between -512 and 512(middle state of the joystick = 0)
}

short JoyStick::y_axis() {
	short y_state = analogRead(VRY_pin); // Value between 0 and 1023
	
	// the value x_state can be 0-1023(1024 states)
	return y_state - 1023/2;	// We will return a value between -512 and 512(middle state of the joystick = 0)
}
