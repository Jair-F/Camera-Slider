#pragma once
#include <Arduino.h>
#include <types.hpp>
#include "path.hpp"
#include "Motor_Nema17.hpp"
#include "joystick.hpp"
#include "exception.hpp"

#include "constants.hpp"

class Slider
{
private:
	Motor_Nema17 *xAxis = nullptr, *yAxis = nullptr, *zAxis = nullptr;
	bool inSyncMoving = false;
	Path *targetPath = nullptr; // if set synchronous movement this is the path
	uint8_t targetPosIndex = 0;
	int16_t endTargetPosIndex = -1;

protected:
	void moveTo(Position *target);
	void step(Motor_Nema17 *_axis, POS_TYPE _numSteps);

	/**
	 * @return searches the next position in targetPath which is marked as set/valid - starting checking at _pos -
	 * 			returns _pos if _pos is already set/valid
	 * @param inverse false to search ascending if _pos is not set. true to search descending if _pos is not set.
	 * @note throws out_of_range if it gets to the end/begin and didnt found a set/valid position and return _pos
	 * @note throws invalid_argument if targetPath is nullptr and return _pos
	 */
	uint8_t getSetMarkedPos(uint8_t _pos, bool inverse = false);

public:
	Slider(Motor_Nema17 *_xAxis, Motor_Nema17 *_yAxis, Motor_Nema17 *_zAxis);
	virtual ~Slider() {}

	/**
	 * @return get the current position of the slider
	 * @note duration and isSet of the Position will be set to 0/false
	 */
	Position getPosition() const;

	/**
	 * @brief set the current position the slider is standing on to _pos
	 */
	void overWritePos(const Position &_pos);

	// allow only if not in sync moving
	void stepX(POS_TYPE _numSteps) { this->step(this->xAxis, _numSteps); }
	void stepY(POS_TYPE _numSteps) { this->step(this->yAxis, _numSteps); }
	void stepZ(POS_TYPE _numSteps) { this->step(this->zAxis, _numSteps); }

	inline void setPath(Path *_targetPath);
	inline Path *getPath() const { return this->targetPath; }

	inline bool getInSyncMoving() const { return this->inSyncMoving; }
	// if the Slider is in sync moving get the next target position index
	inline uint8_t getSyncMoveProgress() const { return this->targetPosIndex; }
	/**
	 * @param _endPos negative for last pos of targetPath
	 * @note throws out_of_range if _targetPath, _startingPos or _endPos arent valid
	 */
	void startSyncMoving(uint8_t _startingPos = 0, int16_t _endPos = -1);
	/**
	 * @param _endPos negative for last pos of targetPath
	 * @note throws out_of_range if _targetPath, _startingPos or _endPos arent valid
	 */
	void startSyncMoving(Path *_targetPath, uint8_t _startingPos = 0, int16_t _endPos = -1);
	inline void stopSyncMoving() { this->inSyncMoving = false; }
	/**
	 * runs sync moving of targetPath if targetPath is set and in
	 * sync moving mode(startSyncMoving was called)
	 */
	void loop();
};

Slider::Slider(Motor_Nema17 *_xAxis, Motor_Nema17 *_yAxis, Motor_Nema17 *_zAxis) : xAxis(_xAxis), yAxis(_yAxis), zAxis(_zAxis)
{
}

uint8_t Slider::getSetMarkedPos(uint8_t _pos, bool inverse)
{
	if (this->targetPath != nullptr)
	{
		if (_pos >= this->targetPath->size())
		{
			throwException(new out_of_range(String(F("passed _pos is out of range - ")) + String(_pos)));
			return _pos;
		}

		// _pos < this->targetPath->size() --> checked in the if above
		if (this->targetPath->at(_pos).isSet)
			return _pos;
		// else if (_pos == this->targetPath->size() - 1 && inverse == true && this->targetPath->at(_pos).isSet)
		//	return _pos; // if at last point and not searching descending (-)

		if (inverse) // descending
		{
			if (_pos == 0)
			{
				throwException(new out_of_range(F("reached begin and didnt found next set pos")));
				return _pos;
			}

			do
			{
				--_pos;
			} while (!this->targetPath->at(_pos).isSet && _pos > 0);
		}
		else // ascending
		{
			if (_pos == this->targetPath->size() - 1)
			{
				throwException(new out_of_range(F("reached end and didnt found next set pos")));
				return _pos;
			}

			do
			{
				++_pos;
			} while (!this->targetPath->at(_pos).isSet && _pos < this->targetPath->size() - 1);
		}

		if ((_pos == 0 || _pos >= this->targetPath->size() - 1))
		{
			//_pos = _pos == 0 ? 0 : this->targetPath->size() - 1; // reset in case it exceeded but leave 0 if _pos is 0
			// if at begin or and and didnt found an element in that range
			throwException(new out_of_range(F("no set element found in search")));
		}
	}
	else
		throwException(new invalid_argument(F("path cant be nullptr for seraching next pos")));

	return _pos; // only that compiler doesnt outputs a warning
}

Position Slider::getPosition() const
{
	Position ret;
	ret.x = this->xAxis->getPos();
	ret.y = this->yAxis->getPos();
	ret.z = this->zAxis->getPos();
	ret.duration = 0;
	ret.isSet = false;
	return ret;
}

void Slider::overWritePos(const Position &_pos)
{
	this->xAxis->overWritePos(_pos.x);
	this->yAxis->overWritePos(_pos.y);
	this->zAxis->overWritePos(_pos.z);
}

void Slider::step(Motor_Nema17 *_axis, POS_TYPE _numSteps)
{
	if (_numSteps > 0)
		for (POS_TYPE i = 0; i < _numSteps; ++i)
			_axis->stepForward();
	else if (_numSteps < 0)
		for (POS_TYPE i = 0; i < abs(_numSteps); ++i)
			_axis->stepBackward();
}

inline void Slider::setPath(Path *_targetPath)
{
	if (_targetPath != nullptr)
	{
		this->targetPath = _targetPath;

		uint8_t startPosIndex = 255; // at ++startPosIndex it will overflow to 0
		uint8_t endPosIndex = 0;

		// check for valid speeds
		while (startPosIndex < this->targetPath->size() - 1 &&
			   endPosIndex < this->targetPath->size() - 1)
		{
			do
			{
				++startPosIndex;
			} while (!this->targetPath->at(startPosIndex).isSet &&
					 startPosIndex < this->targetPath->size() - 1);

			endPosIndex = startPosIndex;
			do
			{
				++endPosIndex;
			} while (!this->targetPath->at(endPosIndex).isSet &&
					 endPosIndex < this->targetPath->size() - 1);

			auto startPos = this->targetPath->at(startPosIndex);
			auto endPos = this->targetPath->at(endPosIndex);
			if (!this->xAxis->isValidSpeed(abs(endPos.x - startPos.x), startPos.duration) ||
				!this->yAxis->isValidSpeed(abs(endPos.y - startPos.y), startPos.duration) ||
				!this->zAxis->isValidSpeed(abs(endPos.z - startPos.z), startPos.duration))
			{
				String msg = F("Slider - Invalid speed between position ");
				msg += startPosIndex;
				msg += F(" and ");
				msg += endPosIndex;
				throwException(new invalid_argument(msg));
				return;
			}
		}
	}
	else
	{
		throwException(new invalid_argument(F("in class Slider setPath-target path cant be nullptr")));
		return;
	}

#ifdef PRINT_DEBUG
	Serial.println(F("path: "));
	for (uint8_t i = 0; i < this->targetPath->size(); ++i)
	{
		auto pos = this->targetPath->at(i);
		Serial.print(pos.isSet);
		Serial.print(F(" -> "));
		Serial.print(i);
		Serial.print(F(": ("));
		Serial.print(pos.x);
		Serial.print(F(", "));
		Serial.print(pos.y);
		Serial.print(F(", "));
		Serial.print(pos.z);
		Serial.print(F(") - duration: "));
		Serial.println(static_cast<unsigned long>(pos.duration));
	}
#endif
}

void Slider::startSyncMoving(uint8_t _startingPos, int16_t _endPos)
{
#ifdef PRINT_DEBUG
	Serial.println(F("Starting sync moving: "));
#endif

	if (this->targetPath != nullptr)
	{
		if (_endPos < 0)
			_endPos = this->targetPath->size() - 1;
	}
	else
	{
		throwException(new invalid_argument(F("path cant be nullptr if starting sync moving")));
		return;
	}

	if (_startingPos > _endPos)
	{
		throwException(new out_of_range(F("not satisfied: _startingPos <= _endPos")));
		return;
	}

	// this->getSetMarkedPos() checks for nullptr int this->targetPosIndex and pos out of range and throws exception if not set
	this->targetPosIndex = this->getSetMarkedPos(_startingPos, false);
	if (!thrownException->isCatched())
		return; // no point to drive to
	this->endTargetPosIndex = this->getSetMarkedPos(_endPos, true);
	if (!thrownException->isCatched())
		return; // no point to drive to

#ifdef PRINT_DEBUG
	Serial.print(F("start point: "));
	Serial.print(this->targetPosIndex);
	Serial.print(F(", end point: "));
	Serial.println(this->endTargetPosIndex);
#endif
	if (targetPosIndex > endTargetPosIndex)
	{
		throwException(new out_of_range(F("at least one point in path must be set in order to start sync moving")));
		return;
	}

	this->inSyncMoving = true;

	auto targetStartPos = this->targetPath->at(this->targetPosIndex);

	// setup the first movment to get to target start position at max speed but only if we arent on that pos
	if (this->getPosition() != targetStartPos)
	{
		auto currentPos = this->getPosition();

		TIME_TYPE maxDuration = 0;

		TIME_TYPE tmp = this->xAxis->minReqTime(abs(currentPos.x - targetStartPos.x));
		maxDuration = tmp > maxDuration ? tmp : maxDuration;

		tmp = this->yAxis->minReqTime(abs(currentPos.y - targetStartPos.y));
		maxDuration = tmp > maxDuration ? tmp : maxDuration;

		tmp = this->zAxis->minReqTime(abs(currentPos.z - targetStartPos.z));
		maxDuration = tmp > maxDuration ? tmp : maxDuration;

		maxDuration += 500000; // add .5 sec spare to be safe with execution time that all motors start

		// could be that the duration is so smal that not all motors can not start sync moving because they check the speed internally again which depend on the time point
		auto now = micros64();
		this->xAxis->startSyncMoving(targetStartPos.x, now + maxDuration);
		this->yAxis->startSyncMoving(targetStartPos.y, now + maxDuration);
		this->zAxis->startSyncMoving(targetStartPos.z, now + maxDuration);
	}
}

void Slider::startSyncMoving(Path *_targetPath, uint8_t _startingPos, int16_t _endPos)
{
	this->setPath(_targetPath);
	if (!thrownException->isCatched())
		return; // an exception was thrown previously
	this->startSyncMoving(_startingPos, _endPos);
}

void Slider::loop()
{
	if (this->inSyncMoving)
	{
		bool allMotorsOnTarget = this->xAxis->onTarget() && this->yAxis->onTarget() && this->zAxis->onTarget();

		// set the next point or stop sync moving if on last point
		if (allMotorsOnTarget)
		{
#ifdef PRINT_DEBUG
			Serial.println(F("all motors on target..."));
			Serial.println(F("searching for next pos in path"));
#endif
			if (this->targetPosIndex == this->endTargetPosIndex)
			{
				this->stopSyncMoving();
#ifdef PRINT_DEBUG
				Serial.println(F("stopping sync moving - reached last point"));
#endif
				return;
			}
			else
			{
				// serach for next set position
				this->targetPosIndex = this->getSetMarkedPos(this->targetPosIndex + 1, false); // search ascending
			}

#ifdef PRINT_DEBUG
			Serial.println(F("next pos or exception!!"));
#endif

			catchException(
				out_of_range(), [](const exception &ex, void *_spareArgument)
				{
#ifdef PRINT_DEBUG
				Serial.println(F("stopping sync moving - reached last point"));
#endif
					// a bit sketchy but it works!!
					Slider* _this = static_cast<Slider*>(_spareArgument);
					_this->stopSyncMoving(); 
					; },
				this);
			if (!this->getInSyncMoving())
				return; // if we are not in sync moving its because the exception above - exit because down we set inSyncMoving again to true

#ifdef PRINT_DEBUG
			Serial.print(F("reached point -> seting next target point: "));
			Serial.print(targetPosIndex);
			Serial.print(F(", duration: "));
			Serial.println(static_cast<unsigned long>(this->targetPath->at(this->targetPosIndex).duration));
#endif
			auto nextTargetPos = this->targetPath->at(this->targetPosIndex);
			auto now = micros64();
			this->xAxis->startSyncMoving(nextTargetPos.x, now + nextTargetPos.duration);
			this->yAxis->startSyncMoving(nextTargetPos.y, now + nextTargetPos.duration);
			this->zAxis->startSyncMoving(nextTargetPos.z, now + nextTargetPos.duration);
			this->inSyncMoving = true;
		}
	}

	this->xAxis->loopSyncMoving();
	this->yAxis->loopSyncMoving();
	this->zAxis->loopSyncMoving();

#ifdef PRINT_DEBUG
	auto pos = this->getPosition();
	Serial.print(F("pos: ("));

	Serial.print(pos.x);
	Serial.print(F(" t: "));
	Serial.print(this->xAxis->onTarget());
	Serial.print(F(", "));

	Serial.print(pos.y);
	Serial.print(F(" t: "));
	Serial.print(this->yAxis->onTarget());
	Serial.print(F(", "));

	Serial.print(pos.z);
	Serial.print(F(" t: "));
	Serial.print(this->zAxis->onTarget());
	Serial.print(F(")"));

	Serial.println();
	Serial.flush();
#endif
}
