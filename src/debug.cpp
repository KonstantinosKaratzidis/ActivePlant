#include "config.h"
#include "debug.hpp"

void TestPin::init(){
	TEST_PIN_DDR |= (1 << TEST_PIN);
}

void TestPin::on(){
	TEST_PIN_PORT |= (1 << TEST_PIN);
}

void TestPin::off(){
	TEST_PIN_PORT &= ~(1 << TEST_PIN);
}

void TestPin::toggle(){
	TEST_PIN_PORT ^= (1 << TEST_PIN);
}

TestPin test_pin;
