#pragma once
#include <Arduino.h>
#include <math.h>
#include "micros_64.hpp"

#define POS_TYPE uint32_t
#define TIME_TYPE uint64_t

namespace
{
	constexpr auto MAX_SPEED = 400; // RPM
	constexpr auto STEPS_PER_ROTATION = 200;

	//												max rotations per minute

	constexpr auto MAX_STEPS_PER_SECOND = (((uint32_t)MAX_SPEED * (uint32_t)STEPS_PER_ROTATION) / 60);
	constexpr auto MIN_DELAY_PER_STEP = ((uint32_t)1000 * (uint32_t)1000) / (uint32_t)MAX_STEPS_PER_SECOND; // in micro seconds
}

class Motor_Nema17
{
private:
	uint8_t enablePin, directionPin, stepPin;
	TIME_TYPE lastStep_TPoint; // in micros
	POS_TYPE pos;
	bool in_syncMovingMode; // if the motor is in synced synchronized mode - approaching a point by a specific time point

	/**
	 * start postion where started with syncMoving
	 */
	POS_TYPE syncMovingStart = 0, syncMovingEnd = 0;
	TIME_TYPE syncMovingStart_TPoint = 0, syncMovingEnd_TPoint = 0;

protected:
	/**
	 * calculates the parameters required by synced moving
	 */
	void calcSyncMoving();

	POS_TYPE getSyncMoveDist();

	// returns the duration of the synchronized move
	TIME_TYPE getSyncMoveDuration() { return this->syncMovingEnd_TPoint - this->syncMovingStart_TPoint; }

	/**
	 * when in syncMoving this function returns the expected position where the motor
	 * should be according the the actual time.
	 *
	 * the actual pos and expected will be compensated and reduced by one at the next
	 * loopSyncMoving() call.
	 */
	POS_TYPE syncMoving_expectedPos();

public:
	Motor_Nema17() {}
	virtual ~Motor_Nema17() {}

	// step one step forward -> in + direction
	void stepForward();
	// step one step backward -> in - direction
	void stepBackward();

	POS_TYPE getPos() const { return pos; }
	void overWritePos(POS_TYPE _pos) { pos = _pos; }

	/**
	 * @return absolute distance from current position to destination
	 */
	POS_TYPE distTo(POS_TYPE _dest);

	/**
	 * @param dist distance for the motor to travel in steps/microsteps
	 * @param duration duration to travel the dist in micro seconds
	 * @return true if the calculated speed doesnt exceeds the max speed of the motor
	 */
	bool isValidSpeed(POS_TYPE dist, TIME_TYPE duration) { return duration / dist > MIN_DELAY_PER_STEP; }

	/**
	 * @return the minimum requred time to travel the pased distance at the motors max speed in micro seconds
	 */
	TIME_TYPE minReqTime(POS_TYPE dist) { return (dist / MAX_STEPS_PER_SECOND) * 1000 * 1000; }

	/**
	 * takes the current position and prepares the synced moving.
	 * by the next loopSyncMoving() call the synced moving will start
	 *
	 * @return true if every parameters are valid - speed in limit and _arrive_TPoint later than now
	 */
	bool startSyncMoving(POS_TYPE _destPos, TIME_TYPE _arrive_TPoint);

	void abortSyncMoving() { this->in_syncMovingMode = false; }

	void loopSyncMoving();
};

void Motor_Nema17::stepForward()
{
	// check that the last step is in more than min delay between step - 5us
	this->pos += 1;
	lastStep_TPoint = micros64();
}

void Motor_Nema17::stepBackward()
{
	// check that the last step is in more than min delay between step - 5us
	this->pos -= 1;
	lastStep_TPoint = micros64();
}

POS_TYPE Motor_Nema17::distTo(POS_TYPE _dest)
{
	if (_dest > this->getPos())
		return _dest - this->getPos();
	else
		return this->getPos() - _dest;
}

POS_TYPE Motor_Nema17::getSyncMoveDist()
{
	// checking becuase its unsigned variable
	if (this->syncMovingEnd > this->syncMovingStart)
		return this->syncMovingEnd - this->syncMovingStart;
	else
		return this->syncMovingStart - this->syncMovingEnd;
}

bool Motor_Nema17::startSyncMoving(POS_TYPE _destPos, TIME_TYPE _arrive_TPoint)
{
	if (_destPos == this->getPos() ||
		_arrive_TPoint <= micros64() ||
		!this->isValidSpeed(this->distTo(_destPos), _arrive_TPoint - micros64()))
		return false;

	this->in_syncMovingMode = true;
	this->syncMovingStart = this->getPos();
	this->syncMovingEnd = _destPos;
	this->syncMovingStart_TPoint = micros64();
	this->syncMovingEnd_TPoint = _arrive_TPoint;

	return true;
}

POS_TYPE Motor_Nema17::syncMoving_expectedPos()
{
	TIME_TYPE microsPerStep = // delay in microseconds between each step
		this->getSyncMoveDuration() / this->getSyncMoveDist();

	if (this->getPos() < this->syncMovingEnd) // moving in + direction
		return this->syncMovingStart + (micros64() - this->syncMovingStart_TPoint) * microsPerStep;
	else // this->syncMovingEnd > this->getPos() -> moving in - direction
		return this->syncMovingStart - (micros64() - this->syncMovingStart_TPoint) * microsPerStep;
}

void Motor_Nema17::loopSyncMoving()
{
	if (this->in_syncMovingMode)
	{
		auto expectedPos = this->syncMoving_expectedPos();
		auto posDiff = this->distTo(expectedPos);
		if (posDiff != 0)
		{
			if (expectedPos > this->getPos()) // moving in + direction
				for (uint8_t i = 0; i < posDiff; ++i)
					this->stepForward();
			else // moving in - direction
				for (uint8_t i = 0; i < posDiff; ++i)
					this->stepBackward();
		}
	}
}
