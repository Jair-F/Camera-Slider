#pragma once
#include <Arduino.h>

class exception
{
private:
	String msg;

public:
	exception(String _msg) noexcept : msg(_msg) {}
	virtual ~exception() {}

	virtual const String what() const noexcept { return this->msg; }
	virtual const char *whatc() const noexcept { return this->msg.c_str(); }
};

class logic_error : public exception
{
public:
	logic_error(String _msg) : exception(_msg) {}
	virtual ~logic_error() {}
};

class runtime_error : public exception
{
public:
	runtime_error(String _msg) : exception(_msg) {}
	virtual ~runtime_error() {}
};

class out_of_range : public logic_error
{
public:
	out_of_range(String _msg) : logic_error(_msg) {}
	virtual ~out_of_range() {}
};

class invalid_argument : public logic_error
{
public:
	invalid_argument(String _msg) : logic_error(_msg) {}
	virtual ~invalid_argument() {}
};
