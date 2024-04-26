#pragma once
#include <Arduino.h>
#include <math.h>
#include "types.hpp"
#include "micros_64.hpp"

namespace
{
	constexpr auto MAX_SPEED = 1200; // RPM
	constexpr auto STEPS_PER_ROTATION = 200;

	//												max rotations per minute
	constexpr auto MAX_STEPS_PER_SECOND = (((uint32_t)MAX_SPEED * (uint32_t)STEPS_PER_ROTATION) / 60);
	constexpr auto MIN_DELAY_PER_STEP = ((uint32_t)1000 * (uint32_t)1000) / (uint32_t)MAX_STEPS_PER_SECOND; // in micro seconds
}

class Motor_Nema17
{
private:
	uint8_t directionPin, stepPin, enablePin = true;
	/**
	 * true on forward, false on backward
	 */
	bool currDirect = true;
	TIME_TYPE lastStep_TPoint = 0;	// in micros
	POS_TYPE pos = 0;				// current pos of motor
	bool in_syncMovingMode = false; // if the motor is in synced/synchronized mode - approaching a point by a specific time point

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

	/**
	 * @return the absolute distance from the point the motor
	 * started(!not necessarily current pos) sync moving to its destination
	 */
	inline POS_TYPE getSyncMoveDist() const { return abs(this->syncMovingEnd - this->syncMovingStart); }

	/**
	 * @return duration of the synchronized move
	 */
	inline TIME_TYPE getSyncMoveDuration() const { return this->syncMovingEnd_TPoint - this->syncMovingStart_TPoint; }

	/**
	 * when in syncMoving this function returns the expected position where the motor
	 * should be according the the actual time.
	 *
	 * the actual pos and expected will be compensated and reduced by one at the next
	 * loopSyncMoving() call.
	 */
	POS_TYPE syncMovingExpectedPos();

	/**
	 * makes one step in the direction the dirPin is set to.
	 *
	 * @attention make sure that between every call of the function there
	 * is at least 5us delay to let the motor work properly
	 */
	inline void _makeStep();

public:
	Motor_Nema17(uint8_t _directionPin, uint8_t _stepPin);
	virtual ~Motor_Nema17() {}

	Motor_Nema17(const Motor_Nema17 &) = delete;
	Motor_Nema17 &operator=(const Motor_Nema17 &) = delete;

	/**
	 * configure the set outputs as output at arduino
	 */
	void begin();

	// step one step forward -> in + direction
	inline void stepForward();
	// step one step backward -> in - direction
	inline void stepBackward();

	// @return current position of motor
	inline POS_TYPE getPos() const { return this->pos; }
	// in order to set manually 0 at a specific point as reference
	inline void overWritePos(POS_TYPE _pos) { this->pos = _pos; }

	/**
	 * @return absolute distance from current position to destination
	 */
	inline POS_TYPE absDistTo(POS_TYPE _dest) const { return abs(_dest - this->getPos()); }

	/**
	 * @return relative distance to _dest -> considers the direction the motor has to go.
	 * - for backward, + for forward
	 */
	inline POS_TYPE relDistTo(POS_TYPE _dest) const { return _dest - this->getPos(); }

	/**
	 * @param dist distance for the motor to travel in steps/microsteps
	 * @param duration duration to travel the dist in micro seconds
	 * @return true if the calculated speed doesnt exceeds the max speed of the motor
	 */
	inline bool isValidSpeed(POS_TYPE dist, TIME_TYPE duration) const { return (duration / dist) > MIN_DELAY_PER_STEP; }

	/**
	 * @return the minimum requred time to travel the pased distance at the motors max speed in micro seconds
	 */
	inline TIME_TYPE minReqTime(POS_TYPE dist) const { return (dist / MAX_STEPS_PER_SECOND) * 1000 * 1000; }

	/**
	 * takes the current position and prepares the synced moving.
	 * by the next loopSyncMoving() call the synced moving will start
	 * @param _arrive_TPoint time point in micro seconds
	 *
	 * @return true if every parameters are valid - speed in limit and _arrive_TPoint later than now
	 */
	bool startSyncMoving(POS_TYPE _destPos, TIME_TYPE _arrive_TPoint);

	inline void abortSyncMoving() { this->in_syncMovingMode = false; }

	/**
	 * should be called on the main loop. moves the
	 * motor if he is in synced moving mode.
	 */
	void loopSyncMoving();

	/**
	 * @return true if the motor's position is on the set target position
	 */
	inline bool onTarget() const { return this->getPos() == this->syncMovingEnd; }

	inline bool isSyncMoving() const { return this->in_syncMovingMode; }
};

Motor_Nema17::Motor_Nema17(uint8_t _directionPin, uint8_t _stepPin) : directionPin(_directionPin), stepPin(_stepPin)
{
}
void Motor_Nema17::begin()
{
	pinMode(directionPin, OUTPUT);
	pinMode(stepPin, OUTPUT);

	digitalWrite(directionPin, this->currDirect);
}

void Motor_Nema17::_makeStep()
{
	digitalWrite(this->stepPin, HIGH);
	delayMicroseconds(5); // min delay for the motor driver to read the step signal
	digitalWrite(this->stepPin, LOW);

	this->lastStep_TPoint = micros64();
}

void Motor_Nema17::stepForward()
{
	if (currDirect != true)
		digitalWrite(directionPin, HIGH);
	this->_makeStep();

	this->pos += 1;
	// this->lastStep_TPoint = micros64();
}

void Motor_Nema17::stepBackward()
{
	if (currDirect != false)
		digitalWrite(directionPin, LOW);
	this->_makeStep();

	this->pos -= 1;
	// this->lastStep_TPoint = micros64();
}

bool Motor_Nema17::startSyncMoving(POS_TYPE _destPos, TIME_TYPE _arrive_TPoint)
{
	if (_destPos == this->getPos() ||
		_arrive_TPoint <= micros64() ||
		!this->isValidSpeed(this->absDistTo(_destPos), _arrive_TPoint - micros64()))
		return false;

	this->in_syncMovingMode = true;
	this->syncMovingStart = this->getPos();
	this->syncMovingEnd = _destPos;
	this->syncMovingStart_TPoint = micros64();
	this->syncMovingEnd_TPoint = _arrive_TPoint;

	return true;
}

POS_TYPE Motor_Nema17::syncMovingExpectedPos()
{
	TIME_TYPE microsPerStep = // delay in microseconds between each step
		this->getSyncMoveDuration() / this->getSyncMoveDist();

	POS_TYPE expectedPos = 0;

	if (this->getPos() < this->syncMovingEnd) // moving in + direction
		expectedPos = this->syncMovingStart + (micros64() - this->syncMovingStart_TPoint) / microsPerStep;
	else // this->getPos() > this->syncMovingEnd -> moving in - direction
		expectedPos = this->syncMovingStart - (micros64() - this->syncMovingStart_TPoint) / microsPerStep;

	/*
	if (expectedPos > this->getSyncMoveDist())
		return this->getSyncMoveDist();
	else
	*/
	return expectedPos;
}

void Motor_Nema17::loopSyncMoving()
{
	if (this->in_syncMovingMode)
	{
		auto expectedPos = this->syncMovingExpectedPos();
		auto posDiff = this->relDistTo(expectedPos);
		if (posDiff != 0)
		{
			if (expectedPos > this->getPos()) // moving in + direction
				for (uint16_t i = 0; i < abs(posDiff); ++i)
					this->stepForward();
			else // moving in - direction
				for (uint16_t i = 0; i < abs(posDiff); ++i)
					this->stepBackward();
		}

		if (this->getPos() == this->syncMovingEnd)
			this->in_syncMovingMode = false;
	}
}
