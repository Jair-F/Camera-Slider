#pragma once
#include <Arduino.h>

namespace
{
	void printErrorAndAbort(String file, const uint32_t line)
	{
		if (!Serial)
			Serial.begin(9600);
		Serial.print("ASSERT FAIL IN FILE: ");
		Serial.print(file);
		Serial.print(", LINE: ");
		Serial.println(String(line));
		Serial.flush();
		abort();
	}
}

/**
 * asserts but in case of a fail it prints a message
 * to the serial monitor to make debugging easier :)
 */
#define assert(e, file, line) ((e) ? (void)0 : printErrorAndAbort(file, line))