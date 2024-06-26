#include <Arduino.h>
#include <math.h>

// #define PRINT_DEBUG
// #define SERIAL_DEBUG

#ifndef PRINT_DEBUG
#ifdef SERIAL_DEBUG
#include <avr8-stub.h>
#endif
#endif

#include "exception.hpp"

#include "Motor_Nema17.hpp"
#include "joystick.hpp"
#include "path.hpp"
#include "slider.hpp"
#include "constants.hpp"

enum class Mode
{
	MANUAL,		 // free joystick movment of all axes
	SYNC_MOVING, // auto - drive according to points set in Path
	REF,		 // "manual auto" select a specific point in path to drive to them as reference to adjust them. after the point is selected it switches to SYNC_MOVING to drive to the point.
	PROGRAM		 // mode on display to select which point to overwrite with the current position
};

Motor_Nema17 motorX(10, 9), motorY(10, 9), motorZ(10, 9);
Path path(20);
Slider slider(&motorX, &motorY, &motorZ);

void setup()
{
#ifdef PRINT_DEBUG
	Serial.begin(115200);
	for (uint8_t i = 0; i < 3; ++i)
		Serial.println();
#else
#ifdef SERIAL_DEBUG
	debug_init();
	breakpoint();
#endif
#endif

	randomSeed(analogRead(A0));

	for (uint8_t i = 0; i < path.size(); ++i)
	{
		auto pos = &path.at(i);
		pos->x = random(-10000, 10000);
		pos->y = random(-10000, 10000);
		pos->z = random(-10000, 10000);
		pos->isSet = random(0, 2);
		pos->duration = random(0.5 * _SECOND_TO_MICRO_SECOND_, 15 * _SECOND_TO_MICRO_SECOND_);
	}

	// throwException(out_of_range("exception try"));
	if (!slider.startSyncMoving(&path, 0))
	{
		catchException(out_of_range(), [](const exception &exc, void *)
					   {
#ifdef PRINT_DEBUG
						   Serial.print("\u001b[31mcatched out_of_range exception after start sync moving: " + exc.what());
						   Serial.println(F("\u001b[0m"));
#endif
#ifdef SERIAL_DEBUG
						   debug_message((String(F("catched out_of_range exception after start sync moving: ")) + exc.what()).c_str());
#endif
					   });
	}
}

void loop()
{
	slider.loop();
	thrownException->loop();
}
