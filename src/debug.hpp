#ifndef PRD_DEBUG_H
#define PRD_DEBUG_H
#include "io.hpp"

#if DEBUG
class TestPin : public Device {
public:
	void on();
	void off();
	void toggle();

	void init() override;
};

class DebugUart : public Device {
public:
	void write_byte(char byte);
	void write(const char *buf, int size);
	void printf(const char *fmt, ...);
	void init() override;
};

#else // DEBUG deactivated
class TestPin : public Device {
public:
	inline void on(){}
	inline void off(){}
	inline void toggle(){}
};
class DebugUart : public Device {
public:
	inline void write_byte(char byte);
	inline void write(const char *buf, size_t size){}
	inline printf(const char *fmt, ...){}
};
#endif

#endif // PRD_DEBUG_H
