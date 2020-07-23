#ifndef PRD_DEBUG_H
#define PRD_DEBUG_H
#include "io.hpp"

class TestPin : public Device {
public:
	void on();
	void off();
	void toggle();

	void init() override;
};

extern TestPin test_pin;

#endif // PRD_DEBUG_H
