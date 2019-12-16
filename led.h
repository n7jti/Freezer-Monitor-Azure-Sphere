#pragma once

class Led {
public:
	enum LED_STATE {
		LED_OFF,
		LED_ON,
		LED_BLINK
	};

	Led(int pin);
	~Led();
	bool begin();
	LED_STATE run();
	int setState(LED_STATE state);
private:
	int _fd;
	int _pin;
	LED_STATE _state; 
};