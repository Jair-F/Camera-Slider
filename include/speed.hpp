#pragma once
#include <Arduino.h>

class Speed
{
private:
	/**
	 * maximum delay in microseconds when the speed is slower-delay greater
	 * it is counted as no moving and the motor will stop moving
	 */
	static constexpr uint32_t _maxDelayMoving = (uint32_t)1000 * 1000 * 60; // one minute
	uint64_t speed;

	/**
	 * type: microseconds
	 * if comparing speeds this is the tolerance if the speeds differ
	 * in the range of +/-_speedComparisionTolerance one from each other
	 * its considered as the same.
	 */
	static constexpr uint16_t _speedComparisionTolerance = 10;

public:
	Speed(uint64_t _speed) : speed(_speed) {}
	virtual ~Speed() {}

	/**
	 * @return true if the delay between the steps is more than
	 * _maxDelayMoving - the motor is considerd as not moving/idle
	 */
	bool idle() const { return this->speed > this->_maxDelayMoving; }
	/**
	 * @return true if the motor is moving - !idle()
	 */
	bool running() const { return !this->idle(); }

	const decltype(speed) value() const { return this->speed; }
	void setIdle() { this->speed = this->_maxDelayMoving + 1; }

	bool
	operator>(uint64_t _speed) const
	{
		return this->speed > _speed;
	}
	bool operator<(uint64_t _speed) const { return this->speed < _speed; }
	bool operator==(uint64_t _speed) const { return this->speed == _speed; }
	bool operator!=(uint64_t _speed) const { return !this->operator==(_speed); }

	Speed &operator+=(uint64_t _speed)
	{
		this->speed += _speed;
		return *this;
	}
	Speed &operator-=(uint64_t _speed)
	{
		this->speed -= _speed;
		return *this;
	}

	/**
	 * @return true if _speed is approximiate the same with tolerance _speedComparisionTolerance
	 * in + and -.
	 */
	bool approxSame(uint64_t _speed) const;

	operator uint64_t() const { return this->speed; }

	const decltype(_maxDelayMoving) maxDelayMoving() const { return this->_maxDelayMoving; }
};

bool Speed::approxSame(uint64_t _speed) const
{
	if (this->speed > _speed)
	{
		return this->speed - _speed <= this->_speedComparisionTolerance;
	}
	else
	{
		return _speed - this->speed <= this->_speedComparisionTolerance;
	}
}
