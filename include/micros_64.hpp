#pragma once
#include <Arduino.h>

/**
 * in order to keep track and dont miss a overflow this
 * function need to be called every 70 minutes
 */
uint64_t micros64()
{
	static uint32_t low32 = 0, high32 = 0;
	uint32_t new_low32 = micros();

	/**
	 * detect if micros had overflow since the last call and if had
	 * and keep track with the first section
	 */
	if (new_low32 < low32)
		++high32;
	return static_cast<uint64_t>(high32) << 32 | low32;
}