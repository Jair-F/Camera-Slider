#pragma once
#include <Arduino.h>
#include "exception.hpp"
#include "msgAssert.hpp"
#include "types.hpp"
#include "position.hpp"

class Path
{
private:
	uint8_t len;
	Position *positions;

public:
	Path(uint8_t _len);
	virtual ~Path() { delete[] positions; }

	/**
	 * @return the duration the movment between positions
	 * startPos and endPos considering the Slider is on startPos
	 *
	 * starPos and EndoPos are the indexes of those positions. Not the
	 * pos on the slide on the slider. Not set positions are ignored
	 *
	 * @note return 0 and throws an exception if startPos or endPos are not valid
	 */
	TIME_TYPE getDuration(uint8_t startPos, uint8_t endPos) const;
	// @return get duration of the whole path - 0 - size - 1 considering the Slider is at position[0]
	TIME_TYPE getDuration() const { return this->getDuration(0, this->len - 1); }

	Position &operator[](uint8_t _pos) const;
	Position &at(uint8_t _pos) const { return this->operator[](_pos); }

	// num of positions in Path
	const uint8_t size() const { return this->len; }
};

Path::Path(uint8_t _len) : len(_len), positions(new Position[_len])
{
}

Position &Path::operator[](uint8_t _pos) const
{
	if (_pos >= this->len)
	{
		throwException(new out_of_range(String(F("_pos is out of range in ")) + __FILE__ + __LINE__));
		return this->positions[0];
	}
	return this->positions[_pos];
}

TIME_TYPE Path::getDuration(uint8_t startPos, uint8_t endPos) const
{
	if (startPos >= this->len || endPos >= this->len)
	{
		throwException(new out_of_range(String(F("startPos or endPos is out of range in ")) + __FILE__ + __LINE__));
		return 0;
	}

	if (startPos > endPos)
	{
		auto tmp = startPos;
		startPos = endPos;
		endPos = tmp;
	}
	else if (startPos == endPos)
		return 0;

	TIME_TYPE retDuration = 0;

	for (decltype(this->len) i = startPos; i < endPos - 1; ++i)
	{
		auto pos = this->positions[i];
		if (pos.isSet)
			retDuration += pos.duration;
	}
	return retDuration;
}
