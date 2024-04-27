#pragma once
#include <Arduino.h>
#include "types.hpp"

class Position
{
public:
	POS_TYPE x = 0, y = 0, z = 0;
	TIME_TYPE duration; // duration to the next pos in micro seconds
	bool isSet = false;

	/**
	 * @note considers only the x,y,z valuse. not the isSet and duration
	 */
	bool operator==(const Position &_pos);

	/**
	 * @note considers only the x,y,z valuse. not the isSet and duration
	 */
	bool operator!=(const Position &_pos);
};

bool Position::operator==(const Position &_pos)
{
	return this->x == _pos.x && this->y == _pos.y && this->z == _pos.z;
}

bool Position::operator!=(const Position &_pos)
{
	return !this->operator==(_pos);
}
