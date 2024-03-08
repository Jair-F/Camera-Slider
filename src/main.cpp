#include <Arduino.h>
#include "Motor_Nema17.hpp"

Motor_Nema17 motor = Motor_Nema17(10, 9);

void setup()
{
	Serial.begin(9600);
	pinMode(10, OUTPUT); // DIR
	pinMode(9, OUTPUT);	 // step
	pinMode(A0, INPUT);	 // joystick nema 23

	for (uint16_t j = 0; j < 4000; ++j)
	{
		motor.stepBackward();
		delayMicroseconds(500);
	}

	motor.overWritePos(0);
	motor.startSyncMoving(4000, micros64() + 5000000);

	while (motor.isSyncMoving())
		motor.loopSyncMoving();
}

void loop()
{
	auto stick_pos = analogRead(A0);
	if (stick_pos < 1023 / 2 - 40 || stick_pos > 1023 / 2 + 40)
	{
		if (stick_pos < 1023 / 2)
		{
			motor.stepForward();
		}
		else
		{
			motor.stepBackward();
		}

		uint16_t delay_micros = 0;
		if (stick_pos > 512)
			delay_micros = 512 - (stick_pos - 512);
		else
			delay_micros = 512 - (512 - stick_pos);

		delayMicroseconds(delay_micros);
	}
}
