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

#endif // PRD_DEBUG_H
