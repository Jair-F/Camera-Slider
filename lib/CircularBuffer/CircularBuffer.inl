#pragma once
#include <Arduino.h>

template <typename T>
class CircularBuffer
{
private:
	const uint16_t len; // of data
	T *data;
	T *iterator;

public:
	CircularBuffer(T _defaultValue, const uint16_t _len);
	virtual ~CircularBuffer();

	void put(T _value);
	/**
	 * @param _index 0 - the last inserted value. 1 - the inserted value before last value .... LENGTH - 1
	 */
	T getValue(uint16_t _index) const;
	uint16_t length() const { return this->len; }
};

template <typename T>
CircularBuffer<T>::CircularBuffer(T _defaultValue, const uint16_t _len) : len(_len), data(nullptr), iterator(nullptr)
{
	this->data = new T[len];

	for (uint16_t i = 0; i < this->len; ++i)
		this->data[i] = _defaultValue;
	this->iterator = data;
}

template <typename T>
CircularBuffer<T>::~CircularBuffer()
{
	if (this->data != nullptr)
		delete[] data;
	this->iterator = nullptr;
}

template <typename T>
void CircularBuffer<T>::put(T _value)
{
	*iterator++ = _value;
}

template <typename T>
T CircularBuffer<T>::getValue(uint16_t _index) const
{
	T *tmpIterator = iterator;
	for (uint16_t i = 0; i < _index; ++i)
	{
		++tmpIterator;
		if (static_cast<uint16_t>(tmpIterator - data) > this->len)
			tmpIterator = data; // reseting to start if going to far - circle
	}
	return *tmpIterator;
}
