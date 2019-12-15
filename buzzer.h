#pragma once

#include <applibs/pwm.h>

class Buzzer
{
public:
	Buzzer(PWM_ControllerId buzzer_controller, PWM_ChannelId buzzer_channel, unsigned int hz);
	~Buzzer();
	bool begin();
	int sound(bool enabled);

private:
	int _fd;
	PWM_ControllerId _controller;
	PWM_ChannelId _channel;
	unsigned int _hz;
};
