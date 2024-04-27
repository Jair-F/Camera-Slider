#pragma once
#include <Arduino.h>

class exception
{
private:
	String msg;
	bool catched;

public:
	exception(String _msg = "", bool _catched = false) : msg(_msg), catched(_catched) {}
	virtual ~exception() {}

	virtual const String what() const;
	virtual const char *whatc() const;

	virtual void markCatched() { this->catched = true; }
	/**
	 * @return true if the last exception is cateched
	 */
	bool isCatched() const { return this->catched; }

	operator bool() const { return this->isCatched(); }
	bool operator!() const { return !this->operator bool(); }

	virtual const String type() const { return F("exception"); }

	void loop();
};

class logic_error : public exception
{
public:
	logic_error(String _msg = "") : exception(_msg) {}
	virtual ~logic_error() {}

	const String type() const override { return F("logic_error"); }
};

class runtime_error : public exception
{
public:
	runtime_error(String _msg = "") : exception(_msg) {}
	virtual ~runtime_error() {}

	const String type() const override { return F("runtime_error"); }
};

class out_of_range : public logic_error
{
public:
	out_of_range(String _msg = "") : logic_error(_msg) {}
	virtual ~out_of_range() {}

	const String type() const override { return F("out_of_range"); }
};

class invalid_argument : public logic_error
{
public:
	invalid_argument(String _msg = "") : logic_error(_msg) {}
	virtual ~invalid_argument() {}

	const String type() const { return F("invalid_argument"); }
};

// implementation

const String exception::what() const
{
	// this->catched = true;
	return this->msg;
}

const char *exception::whatc() const
{
	// this->catched = true;
	return this->msg.c_str();
}

void exception::loop()
{
	if (!this->catched)
	{
#ifdef PRINT_DEBUG
		Serial.println(String(F("uncatched exception(")) + this->type() + String(F("): ")) + this->what() + F(" --> stoping execution"));
		Serial.flush(); // ensure the message will be printed
#endif
		abort(); // stop execution
	}
}

exception *thrownException = new exception("", true);

void throwException(exception *exc)
{
	// first check if the previous was cateched. could be that we didnt passed one loop
	thrownException->loop();

	delete thrownException;

#ifdef PRINT_DEBUG
	Serial.println("throwed exception - " + exc->type() + String(F(" - msg: ")) + exc->what());
#endif
	thrownException = exc;
}

void catchException(const exception &catchType, void (*onExceptHandler)(const exception &, void *_spareArgument) = nullptr, void *_spareArgument = nullptr)
{
	if (thrownException->type() == catchType.type())
	{
		// if the exceptions match and the handler is set - execute
		if (onExceptHandler != nullptr)
			onExceptHandler(*thrownException, _spareArgument);

		thrownException->markCatched();

#ifdef PRINT_DEBUG
		Serial.println("exception - " + thrownException->type() + String(F(" - msg: ")) + thrownException->what() + "catched");
#endif
	}
}
