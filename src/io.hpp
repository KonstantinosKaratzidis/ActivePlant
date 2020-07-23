// defines classes for interacting with low level io
#ifndef PRD_IO_HPP
#define PRD_IO_HPP
#include <stdint.h>
#include <stddef.h>
#include "config.h"

class Device;
class Pump;
class Moisture;
class Usart;

extern Pump pump;
extern Moisture moisture;
extern Usart usart;

// initialize all devices
// this function needs to be called before any device is used
void init_devices();

class Device {
	public:
	virtual void init();
	virtual void sleep();
	virtual void wakeup();
};

class Pump : public Device {
	public:
	enum class State {
		closed,
		open
	};

	Pump();
	void open();
	void close();
	State get_state();

	void init() override;
	void sleep() override;
	void wakeup() override;
	private:
	State _state;
};

class Moisture : public Device {
	public:
	Moisture();
	uint16_t read();

	// max value in range (non inclusive)
	static constexpr uint16_t max_value(){ return 1023; }

	void update(uint16_t sample);

	void init() override;
	void sleep() override;
	void wakeup() override;

	private:
	void set_reference();
	void set_channel(uint8_t channel);
	void enable();
	void disable();
	void set_clk_prescale();
	void enable_interrupt();
	void start_conversion();

	uint16_t samples[ADC_BUF_SZ] = {0};
	uint32_t samples_sum = 0;
	int sample_index = 0;
};

// a driver for the builtin usart
// write is blocking, reading is callback based
// TODO: for now only 9600 and cannot change
class Usart : public Device {
	friend void call_rx_callback();
	public:
	typedef void (*rx_cb_t)(char c);

	Usart();

	// blocking write
	void write(const char *buf, size_t size);

	void init() override;
	void sleep() override;
	void wakeup() override;

	// register a cb to be called every time there is a new character
	void register_rx_cb(rx_cb_t rx_cb);
};

#endif // PRD_IO_HPP
